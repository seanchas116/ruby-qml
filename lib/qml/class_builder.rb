require 'ropework'
require 'qml/qml'

module QML

  class QtProperty < Ropework::Property
    def initialize(obj, metaobj, name)
      @obj = obj
      @metaobj = metaobj
      @name = name
      signal = @metaobj.notify_signal(name)
      metaobj.connect_signal(obj, signal, method(:set_value_orig)) if signal
    end

    alias_method :set_value_orig, :value=

    def value=(newval)
      @metaobj.set_property(@obj, @name, newval)
    end
  end

  class QtSignal < Ropework::Signal
    def initialize(obj, metaobj, name)
      @obj = obj
      @metaobj = metaobj
      @name = name
      metaobj.connect_signal(obj, name, method(:emit_orig))
    end

    alias_method :emit_orig, :emit

    def emit(*args)
      @metaobj.invoke(@name, args)
    end
  end

  class ClassBuilder

    def initialize(metaobj)
      @metaobj = metaobj
    end

    def create_class
      @klass = Class.new(ObjectBase) do
        include Ropework::PropertyDef
        include Ropework::SignalDef
      end
      @metaobj.method_names.each do |name|
        define_method(name)
      end
      @metaobj.property_names.each do |name|
        define_property(name)
      end
      @metaobj.enumerators.each do |k, v|
        const_set(k, v)
      end

      @klass
    end

    private

    def define_method(name)
      metaobj = @metaobj
      if @metaobj.signal?(name)
        @klass.class_eval { signal name, signal: proc { QtSignal.new(self, metaobj, name) } }
      else
        @klass.define_method(name) do |*args|
          metaobj.invoke(name, args)
        end
        @klass.class_eval { private name } if @metaobj.protected?(name)
      end
    end

    def define_property(name)
      @klass.class_eval { property name, property: proc { QtSignal.new(self, metaobj, name) } }
    end
  end
end
