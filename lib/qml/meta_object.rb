require 'qml/qt_object_base'
require 'ropework'

module QML
  class MetaObject

    extend FFI::DataConverter
    native_type FFI::Type::POINTER

    def self.to_native(metaobj, ctx)
      ptr = metaobj.pointer
      fail TypeError, "Null pointer" if ptr.null?
      ptr
    end

    def self.from_native(ptr, ctx)
      self.new(ptr)
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

    def class_name
      CLib.qmetaobject_class_name(self).to_sym
    end

    def method_count
      CLib.qmetaobject_method_count(self)
    end

    def method_name(index)
      CLib.qmetaobject_method_name(self, index).to_sym
    end

    def method_parameter_names(index)
      CLib.qmetaobject_method_parameter_names(self, index).to_a.map(&:to_sym)
    end

    def method_parameter_types(index)
      CLib.qmetaobject_method_parameter_types(self, index).to_a
    end

    def method_signal?(index)
      CLib.qmetaobject_method_is_signal(self, index)
    end

    def invoke_method(object, index, *args)
      param_names = method_parameter_names(index)
      param_types = method_parameter_types(index)
      unless param_types.length == args.length
        fail ArgumentError, "wrong number of arguments for Qt method (#{args.length} for #{param_types.length})"
      end

      args = args.lazy.zip(param_names, param_types).map do |arg, name, type|
        arg = Variant.new(arg)
        arg.convert(type).tap do |converted|
          unless converted.valid?
            fail TypeError, "cannot convert #{Variant.type_name(arg.type_number)} to #{Variant.type_name(type)}"
          end
        end
      end.to_a

      CLib.qmetaobject_method_invoke(self, object.pointer, index, Variant.new(args)).value
    end

    def connect_signal(object, index, &func)
      callback = ->(args) { func.call(*args) }
      CLib.qmetaobject_signal_connect(self, object.pointer, index, callback)
      object.gc_protect(callback)
    end

    def property_count
      CLib.qmetaobject_property_count(self)
    end

    def property_name(index)
      CLib.qmetaobject_property_name(self, index).to_sym
    end

    def property_notify_signal(index)
      CLib.qmetaobject_property_notify_signal(self, index)
    end

    def set_property(object, index, value)
      CLib.qmetaobject_property_set(self, object.pointer, index, Variant.new(value))
    end

    def get_property(object, index)
      CLib.qmetaobject_property_get(self, object.pointer, index).value
    end

    def enum_count
      CLib.qmetaobject_enum_count(self)
    end

    def get_enum(index)
      CLib.qmetaobject_enum_get(self, index).to_hash
    end

    private

      def create_class
        metaobj = self
        Class.new(QtObjectBase) do
          include Ropework::PropertyDef
          include Ropework::SignalDef

          # define methods
          # TODO: support method overloading by number of arguments
          metaobj.method_count.times do |i|
            name = metaobj.method_name(i)
            if metaobj.method_signal?(i)
              signal name, metaobj.method_parameter_names(i)
            else
              define_method(name) do |*args|
                metaobj.invoke_method(self, i, *args)
              end
            end
          end

          # define properties
          metaobj.property_count.times do |i|
            property(metaobj.property_name(i))
              .getter { metaobj.get_property(self, i) }
              .setter { |newval| metaobj.set_property(self, i, newval) }
          end

          # define enums
          metaobj.enum_count.times do |i|
            metaobj.get_enum(i).each do |k, v|
              const_set(k, v)
            end
          end

          define_method(:initialize) do |obj_ptr|
            super(obj_ptr)

            # connect properties
            metaobj.property_count.times do |i|
              property = properties[metaobj.property_name(i)]

              notify_signal_index = metaobj.property_notify_signal(i)

              signal = signals[metaobj.method_name(notify_signal_index)]
              signal.connect do |newval|
                property.changed.emit(newval)
              end
            end

            # connect signals
            metaobj.method_count.times do |i|
              next unless metaobj.method_signal?(i)

              signal = signals[metaobj.method_name(i)]
              metaobj.connect_signal(self, i) do |*args|
                signal.emit(*args)
              end
            end
          end
        end
      end
  end
end