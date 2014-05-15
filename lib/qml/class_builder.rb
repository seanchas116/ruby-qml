require 'logger'
require 'ropework'
require 'qml/object_base'
require 'qml/qml'
require 'qml/null_logger'

module QML

  class QtProperty < Ropework::Property
    def initialize(objptr, metaobj, name)
      super()
      @objptr = objptr
      @metaobj = metaobj
      @name = name
      signal = @metaobj.notify_signal(name)
      metaobj.connect_signal(objptr, signal, method(:set_value_orig)) if signal
      set_value_orig(read_value)
    end

    alias_method :set_value_orig, :value=

    def value=(newval)
      write_value(newval)
    end

    private

    def write_value(newval)
      @metaobj.set_property(@objptr, @name, newval)
    end

    def read_value
      @metaobj.get_property(@objptr, @name)
    end
  end

  class QtSignal < Ropework::Signal
    def initialize(objptr, metaobj, name)
      super(variadic: true)
      @objptr = objptr
      @metaobj = metaobj
      @name = name
      metaobj.connect_signal(objptr, name, method(:emit_orig))
    end

    alias_method :emit_orig, :emit

    def emit(*args)
      @metaobj.invoke(@name, args)
    end
  end

  class ClassBuilder

    attr_reader :metaobj, :klass

    def initialize(metaobj, klass)
      @metaobj = metaobj
      @klass = klass
      #@logger = Logger.new(PROJECT_ROOT + 'log/class_builder.log')
      @logger = NullLogger.new
    end

    def update
      super_metaobj = @metaobj.super_class

      @klass ||= Class.new(super_metaobj ? super_metaobj.object_class : ObjectBase) do
        include Ropework::PropertyDef
        include Ropework::SignalDef
      end

      @logger.info "--------"
      @logger.info "creating class for #{@metaobj.name}"
      @logger.info "--------"

      @logger.info "methods:"
      @logger.info @metaobj.method_names.inspect

      @metaobj.method_names.each do |name|
        define_method(name)
      end

      @logger.info "properties:"
      @logger.info @metaobj.property_names.inspect

      @metaobj.property_names.each do |name|
        define_property(name)
      end
      @metaobj.enumerators.each do |k, v|
        @klass.send :const_set, k, v
      end

      metaobj = @metaobj
      @klass.define_singleton_method :meta_object do
        metaobj
      end

      @klass
    end

    private

    def define_method(name)
      metaobj = @metaobj
      return if @metaobj.private?(name)
      if @metaobj.signal?(name)
        @logger.info "signal: #{name}"
        @klass.send :signal, name, signal: proc { QtSignal.new(@objptr, metaobj, name) }
      else
        @klass.send :define_method, name do |*args|
          metaobj.invoke_method(@objptr, name, args)
        end
        @klass.send :private, name if @metaobj.protected?(name)
      end
    end

    def define_property(name)
      metaobj = @metaobj
      @klass.send :property, name, property: proc { QtProperty.new(@objptr, metaobj, name) }
    end
  end
end
