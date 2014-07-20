require 'logger'
require 'qml/qt_object_base'
require 'qml/qml'
require 'qml/name_helper'

module QML

  class QtProperty
    prepend Reactive::Bindable

    attr_reader :changed

    def initialize(metaobj, objptr, name)
      super()
      @metaobj = metaobj
      @objptr = objptr
      @name = name
      @changed = QtSignal.new(metaobj, objptr, @metaobj.notify_signal(@name))
    end

    def value=(newval)
      @metaobj.set_property(@objptr, @name, newval)
    end

    def value
      @metaobj.get_property(@objptr, @name)
    end
  end

  class QtSignal < Reactive::Signal
    def initialize(metaobj, objptr, name)
      super(variadic: true)
      @objptr = objptr
      @metaobj = metaobj
      @name = name
    end

    private

    def connected(block)
      return if @initialized
      @metaobj.connect_signal(@objptr, @name, method(:emit))
      @initialized = true
    end
  end

  class ClassBuilder

    attr_reader :metaobj, :klass

    def initialize(metaobj, klass)
      @metaobj = metaobj
      @klass = klass
    end

    def build
      create unless @klass
      return self if @klass.meta_object == @metaobj

      properties = @metaobj.property_names
      notifiers = properties.map { |name| @metaobj.notify_signal(name) }
      signals = @metaobj.method_names.select { |name| @metaobj.signal?(name) } - notifiers
      methods = @metaobj.method_names.reject { |name| @metaobj.signal?(name) }
      enums = @metaobj.enumerators

      properties.each do |name|
        define_property(name)
      end
      signals.each do |name|
        define_signal(name)
      end
      methods.each do |name|
        define_method(name)
      end
      enums.each do |k, v|
        define_enum(k, v)
      end
      @klass.__send__ :meta_object=, @metaobj

      self
    end

    private

    def create
      super_metaobj = @metaobj.super_class
      @klass = Class.new(super_metaobj ? super_metaobj.build_class : QtObjectBase)
    end

    def define_method(name)
      metaobj = @metaobj
      return if metaobj.private?(name)
      return if @klass.instance_methods.include?(name)
      @klass.__send__ :define_method, name do |*args|
        metaobj.invoke_method(@pointer, name, args)
      end
      @klass.__send__ :private, name if @metaobj.protected?(name)
      underscore = NameHelper.to_underscore(name)
      @klass.__send__ :alias_method, underscore, name if underscore != name
    end

    def define_signal(name)
      return if @klass.instance_signals.include?(name)
      @klass.__send__ :variadic_signal, name, factory: proc { |obj|
        QtSignal.new(@metaobj, obj.pointer, name)
      }
      underscore = NameHelper.to_underscore(name)
      @klass.__send__ :alias_signal, underscore, name if underscore != name
    end

    def define_property(name)
      return if @klass.instance_properties.include?(name)
      metaobj = @metaobj
      @klass.__send__ :property, name, factory: proc { |obj|
        QtProperty.new(@metaobj, obj.pointer, name)
      }
      underscore = NameHelper.to_underscore(name)
      @klass.__send__ :alias_property, underscore, name if underscore != name
    end

    def define_enum(name, hash)
      define_const(name, hash.values.sort)
      hash.each do |k, v|
        define_const(k, v)
      end
    end

    def define_const(name, value)
      name = (name[0].capitalize + name[1..-1]).to_sym
      @klass.__send__ :const_set, name, value
      underscore = NameHelper.to_upper_underscore(name)
      @klass.__send__ :const_set, underscore, value if underscore != name
    end
  end
end
