require 'qml/qt_object_base'
require 'qml/class_builder'
require 'ropework'

module QML

  class MetaMethod
    attr_reader :meta_object, :index

    def initialize(metaobj, index)
      @meta_object = metaobj
      @index = index
    end

    def name
      @name ||= CLib.rbqml_metaobject_method_name(meta_object, index).to_sym
    end

    def arity
      @arity ||= arg_names.length
    end

    def arg_names
      @arg_names ||= CLib.rbqml_metaobject_method_parameter_names(meta_object, index).to_a.map(&:to_sym)
    end

    def arg_types
      @arg_types ||= CLib.rbqml_metaobject_method_parameter_types(meta_object, index).to_a.map(&MetaType.method(:new))
    end

    def signal?
      @is_signal = CLib.rbqml_metaobject_method_is_signal(meta_object, index) if @is_signal.nil?
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

      CLib.rbqml_metaobject_method_invoke(meta_object, object, index, Variant.new(args)).value
    end

    def connect_signal(object, &func)
      fail TypeError, "#{meta_object.name}::#{name} is not a signal" unless signal?

      callback = ->(args) { func.call(*args) }
      CLib.rbqml_metaobject_signal_connect(meta_object, object, index, callback)
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
      @name ||= CLib.rbqml_metaobject_property_name(meta_object, index).to_sym
    end

    def notify_signal
      @notify_signal ||= MetaMethod.new(meta_object, CLib.rbqml_metaobject_property_notify_signal(meta_object, index))
    end

    def set_value(object, value)
      CLib.rbqml_metaobject_property_set(meta_object, object, index, Variant.new(value))
    end

    def get_value(object)
      CLib.rbqml_metaobject_property_get(meta_object, object, index).value
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
      return nil if ptr.null?
      @metaobjects ||= {}
      @metaobjects[ptr.to_i] ||= self.new(ptr)
    end

    attr_accessor :ruby_class
    attr_reader :pointer

    def self.add(metaobject)
      @metaobjects ||= {}
      @metaobjects[metaobject.pointer.to_i] = metaobject
    end

    def initialize(pointer)
      @pointer = pointer
      @ruby_class = ClassBuilder.new(self).create_class
      self.class.add(self)
    end

    def name
      @name ||= CLib.rbqml_metaobject_class_name(self).to_s
    end

    # @return [Hash{Symbol=>Array<QML::MetaMethod>}]
    def meta_methods(include_super: false)
      @meta_methods ||= begin
        count = CLib.rbqml_metaobject_method_count(self)
        offset = CLib.rbqml_metaobject_method_offset(self)
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
        count = CLib.rbqml_metaobject_property_count(self)
        offset = CLib.rbqml_metaobject_property_offset(self)
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
        count = CLib.rbqml_metaobject_enum_count(self)
        offset = CLib.rbqml_metaobject_enum_offset(self)
        (offset...count).map { |i| CLib.rbqml_metaobject_enum_get(self, i).to_hash }.inject({}, &:merge)
      end

      if include_super && superclass
        superclass.enums(include_super: true).merge(@enums)
      else
        @enums
      end
    end

    def superclass
      @superclass ||= CLib.rbqml_metaobject_super(self)
    end

    def ancestors
      s = superclass
      s ? [self] + s.ancestors : [self]
    end

    def ==(other)
      pointer == other.pointer
    end

    alias_method :eql?, :==

    def hash
      pointer.to_i.hash
    end

    def to_s
      name.to_s
    end

    def inspect
      "<QML::MetaObject:#{name}>"
    end
  end
end