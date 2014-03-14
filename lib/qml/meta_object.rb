require 'qml/qt_object_base'
require 'ropework'

module QML

  class MetaMethod
    attr_reader :meta_object, :index

    def initialize(metaobj, index)
      @meta_object = metaobj
      @index = index
    end

    def name
      @name ||= CLib.qmetaobject_method_name(meta_object, index).to_sym
    end

    def arity
      @arity ||= arg_names.length
    end

    def arg_names
      @arg_names ||= CLib.qmetaobject_method_parameter_names(meta_object, index).to_a.map(&:to_sym)
    end

    def arg_types
      @arg_types ||= CLib.qmetaobject_method_parameter_types(meta_object, index).to_a.map(&MetaType.method(:new))
    end

    def signal?
      @is_signal = CLib.qmetaobject_method_is_signal(meta_object, index) if @is_signal.nil?
      @is_signal
    end

    def invoke(object, *args)
      unless arity == args.length
        fail ArgumentError, "wrong number of arguments for Qt method (#{args.length} for #{arity})"
      end

      args = args.lazy.zip(arg_types).map do |arg, type|
        arg = Variant.new(arg)
        arg.convert(type).tap do |converted|
          unless converted.valid?
            fail TypeError, "cannot convert #{arg.meta_type} to #{type}"
          end
        end
      end.to_a

      CLib.qmetaobject_method_invoke(meta_object, object.pointer, index, Variant.new(args)).value
    end

    def connect_signal(object, &func)
      fail TypeError, "#{meta_object.name}::#{name} is not a signal" unless signal?

      callback = ->(args) { func.call(*args) }
      CLib.qmetaobject_signal_connect(meta_object, object.pointer, index, callback)
      object.gc_protect(callback)
    end
  end

  class MetaProperty
    attr_reader :meta_object, :index

    def initialize(metaobj, index)
      @meta_object = metaobj
      @index = index
    end

    def name
      @name ||= CLib.qmetaobject_property_name(meta_object, index).to_sym
    end

    def notify_signal
      @notify_signal ||= MetaMethod.new(meta_object, CLib.qmetaobject_property_notify_signal(meta_object, index))
    end

    def set_value(object, value)
      CLib.qmetaobject_property_set(meta_object, object.pointer, index, Variant.new(value))
    end

    def get_value(object)
      CLib.qmetaobject_property_get(meta_object, object.pointer, index).value
    end
  end

  class MetaObject

    extend FFI::DataConverter
    native_type FFI::Type::POINTER

    def self.to_native(metaobj, ctx)
      ptr = metaobj.pointer
      fail TypeError, "Null pointer" if ptr.null?
      ptr
    end

    def self.from_native(ptr, ctx)
      ptr.null? ? nil : self.new(ptr)
    end

    attr_reader :pointer

    def self.find(class_name)
      @metaobjects[class_name]
    end

    def self.add(metaobject)
      @metaobjects ||= []
      @metaobjects << metaobject
    end

    def created_class
      @created_class ||= create_class
    end

    def initialize(pointer)
      @pointer = pointer
      self.class.add(self)
    end

    def name
      @name ||= CLib.qmetaobject_class_name(self).to_sym
    end

    # @return [Hash{Symbol=>Array<QML::MetaMethod>}]
    def meta_methods(include_super: false)
      @meta_methods ||= begin
        count = CLib.qmetaobject_method_count(self)
        offset = CLib.qmetaobject_method_offset(self)
        (offset...count).map { |i| MetaMethod.new(self, i) }.group_by(&:name)
      end

      if include_super && superclass
        superclass.meta_methods(include_super: true).merge(@meta_methods)
      else
        @meta_methods
      end
    end

    # @return [Hash{Symbol=>QML::MetaProperty}]
    def meta_properties(include_super: false)
      @meta_properties ||= begin
        count = CLib.qmetaobject_property_count(self)
        offset = CLib.qmetaobject_property_offset(self)
        property_groups = (offset...count).map { |i| MetaProperty.new(self, i) }.group_by(&:name)
        Hash[property_groups.map { |k, v| [k, v.first] }]
      end

      if include_super && superclass
        superclass.meta_properties.merge(@meta_properties)
      else
        @meta_properties
      end
    end

    # @return [Hash{Symbol=>Integer}]
    def enums(include_super: false)
      @enums ||= begin
        count = CLib.qmetaobject_enum_count(self)
        offset = CLib.qmetaobject_enum_offset(self)
        (offset...count).map { |i| CLib.qmetaobject_enum_get(self, i).to_hash }.inject({}, &:merge)
      end

      if include_super && superclass
        superclass.enums(include_super: true).merge(@enums)
      else
        @enums
      end
    end

    def superclass
      @superclass ||= CLib.qmetaobject_super(self)
    end

    def ancestors
      s = superclass
      s ? [self] + s.ancestors : [self]
    end

    protected

      def overridden?(ary, item)
        ary.any? { |each_item| each_item.name == item}
      end

    private

      def create_class
        metaobj = self
        Class.new(QtObjectBase) do
          include Ropework::PropertyDef
          include Ropework::SignalDef

          # define methods
          # TODO: support method overloading
          metaobj.meta_methods(include_super: true).each_value.lazy.map(&:last).each do |m|
            if m.signal?
              signal(m.name, m.arg_names)
            else
              define_method(m.name) do |*args|
                m.invoke(self, *args)
              end
            end
          end

          # define properties
          metaobj.meta_properties(include_super: true).each_value do |p|
            property(p.name)
              .getter { p.get_value(self) }
              .setter { |newval| p.set_value(self, newval) }
          end

          # define enums
          metaobj.enums(include_super: true).each do |k, v|
            const_set(k, v)
          end

          define_method(:initialize) do |obj_ptr|
            super(obj_ptr)

            # connect properties
            metaobj.meta_properties.each_value do |p|
              property = properties[p.name]
              signal = signals[p.notify_signal.name]
              signal.connect do |newval|
                property.changed.emit(newval)
              end
            end

            # connect signals
            metaobj.meta_methods.values.flatten.select(&:signal?).each do |s|
              signal = signals[s.name]
              s.connect_signal(self) do |*args|
                signal.emit(*args)
              end
            end
          end
        end
      end
  end
end