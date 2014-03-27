require 'ropework'

module QML

  class QtPropertyHolder < Ropework::AbstractHolder
    def initialize(obj, metap)
      super()
      @obj = obj
      @metap = metap
      signal = @obj.signals[metap.notify_signal.name]
      signal.connect(&changed.method(:emit)) if signal
    end

    def value
      @metap.get_value(@obj)
    end

    def value=(newval)
      @metap.set_value(@obj, newval)
      value
    end
  end

  class ClassBuilder

    def initialize(metaobj)
      @metaobj = metaobj
    end

    def create_class
      @klass = Class.new(QtObjectBase) do
        include Ropework::PropertyDef
        include Ropework::SignalDef
      end
      define_methods
      define_properties
      define_enums
      define_initialize
      @klass
    end

    private

    def define_methods
      @metaobj.meta_methods(include_super: true).each do |name, methods|
        next if name == :deleteLater # deleteLater must not be called
        if methods.any?(&:signal?)
          @klass.signal(name)
        else
          @klass.send(:define_method, name) do |*args|
            classes = args.map(&:class)
            puts "classes: #{classes}"

            method = methods.find { |method|
              next false unless method.arity == args.length
              arg_classes = method.arg_types.map(&:ruby_class)
              puts "arg_classes: #{arg_classes}"
              arg_classes.zip(classes).all? { |arg_class, given_class|
                arg_class >= given_class
              }
            }
            fail ArgumentError, "no matching method for given parameter types #{classes}" unless method
            method.invoke(self, *args)
          end
        end
      end
    end

    def define_properties
      @metaobj.meta_properties(include_super: true).each_value do |p|
        @klass.property(p.name, holder: ->(obj) { QtPropertyHolder.new(obj, p) })
      end
    end

    def define_enums
      @metaobj.enums(include_super: true).each do |k, v|
        @klass.const_set(k, v)
      end
    end

    def define_initialize
      metaobj = @metaobj
      @klass.send(:define_method, :initialize) do |obj_ptr, destroy: true|
        super(obj_ptr, destroy: destroy)

        # connect properties
        metaobj.meta_properties(include_super: true).each_value do |p|
          property = properties[p.name]
          signal = signals[p.notify_signal.name]
          signal.connect do |newval|
            property.changed.emit(newval)
          end
        end

        # connect signals
        metaobj.meta_methods(include_super: true).each do |name, methods|
          signal = signals[name]
          signal_methods = methods.select(&:signal?)
          signal_methods.each do |signal_method|
            signal_method.connect_signal(self) do |*args|
              signal.emit(*args)
            end
          end
        end
      end
    end
  end
end
