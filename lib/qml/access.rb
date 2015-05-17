module QML

  # {Access} enables classes to be exposed to QML.
  #
  module Access
    # @!parse extend ClassMethods

    # @api private
    class PropertyInfo
      attr_accessor :initializer
    end

    # @api private
    class SignalInfo
      attr_accessor :params
      attr_accessor :listeners

      def initialize
        @listeners = []
      end
    end

    def initialize(*args, &block)

      self.class.property_infos.each do |name, info|
        if info.initializer
          self.__send__ :"#{name}=", instance_eval(&info.initializer)
        end
      end

      self.class.signal_infos.each do |name, info|
        __send__(name).connect do |*args|
          info.listeners.each do |listener|
            instance_exec(*args, &listener)
          end
        end
      end

      super
    end

    def set_signal_emitter(emitter)
      self.class.signals.each do |name|
        __send__(name).connect do |*args|
          emitter.emit(name, args)
        end
      end
    end
    private :set_signal_emitter

    def self.included(derived)
      derived.class_eval do
        extend ClassMethods
      end
    end

    # allowed name patterns for exposed method names
    ALLOWED_PATTERN = /^[a-zA-Z_]\w*$/

    module ClassMethods

      def signal(name, params = [])
        name = name.to_sym
        signal_infos(false)[name] = SignalInfo.new.tap do |info|
          info.params = params.map(&:to_sym)
        end

        class_eval <<-EOS, __FILE__, __LINE__ + 1
          def #{name}
            @_signal_#{name} ||= begin
              args = self.class.signal_infos[:#{name}].params
              Signal.new(args)
            end
          end
        EOS

        name
      end

      private :signal

      def signals(include_super = true)
        signal_infos(include_super).keys
      end

      def signal_infos(include_super = true)
        if include_super && superclass.include?(Access)
          superclass.signal_infos.merge(signal_infos(false))
        else
          @signal_infos ||= {}
        end
      end

      def property(name, &block)
        name = name.to_sym
        signal(:"#{name}_changed", [:"new_#{name}"])

        property_infos(false)[name] = PropertyInfo.new.tap do |info|
          info.initializer = block
        end

        class_eval <<-EOS, __FILE__, __LINE__ + 1
          attr_reader :#{name}
          def #{name}=(new_value)
            new_value = new_value
            if @#{name} != new_value
              @#{name} = new_value
              #{name}_changed.emit(new_value)
            end
          end
        EOS

        name
      end

      private :property

      def properties(include_super = true)
        property_infos(include_super).keys
      end

      def property_infos(include_super = true)
        if include_super && superclass.include?(Access)
          superclass.property_infos.merge(property_infos(false))
        else
          @property_infos ||= {}
        end
      end

      def on(signal, &block)
        info = signal_infos(false)[signal.to_sym] or fail AccessError, "no signal `#{signal}` found"
        info.listeners << block
        block
      end

      def on_changed(property, &block)
        on(:"#{property}_changed", &block)
      end

      private :on, :on_changed

      # Registers the class as a QML type.
      # @param opts [Hash]
      # @option opts [String] :under the namespece which encapsulates the exported QML type. If not specified, automatically inferred from the module nesting of the class.
      # @option opts [String] :version the version of the type. Defaults to VERSION constant of the encapsulating module / class of the class.
      # @option opts [String] :name the name of the type. Defaults to the name of the class.
      def register_to_qml(opts = {})
        metadata = guess_metadata(opts)
        classname = "RubyQml::Access::#{name}"

        exporter = Exporter.new(self, classname)

        signals = self.signals.grep(ALLOWED_PATTERN)
        properties = self.properties.grep(ALLOWED_PATTERN)

        signals.each do |signal|
          exporter.add_signal(signal, signal_infos[signal].params)
        end

        properties.each do |prop|
          exporter.add_property(prop, :"#{prop}_changed")
        end

        methods = ancestors.take_while { |k| k.include?(Access) }
          .map { |k| k.instance_methods(false) }.inject(&:|)
          .grep(ALLOWED_PATTERN)
        ignored_methods = signals | properties.flat_map { |p| [p, :"#{p}=", :"#{p}_changed"] }

        (methods - ignored_methods).each do |method|
          instance_method = self.instance_method(method)
          # ignore variadic methods
          if instance_method.arity >= 0
            exporter.add_method(method, instance_method.arity)
          end
        end

        exporter.register(
          metadata[:under],
          metadata[:versions][0],
          metadata[:versions][1],
          metadata[:name]
        )
      end

      private

      def guess_metadata(opts)
        under = opts[:under]
        version = opts[:version]
        name = opts[:name]

        if !under || !version || !name
          path = self.name.split('::')
        end
        if !under && !version
          fail AccessError, "cannot guess namespace of toplevel class '#{self.name}'" if path.size == 1
          encapsulatings = path[0, path.size - 1]
        end

        under ||= encapsulatings.join('.')
        version ||= eval("::#{encapsulatings.join('::')}").const_get(:VERSION)
        versions = version.split('.').map(&method(:Integer))
        fail AccessError, 'insufficient version (major and minor versions required)' unless versions.size >= 2
        name ||= path.last

        {under: under, versions: versions, name: name}
      end
    end
  end
end
