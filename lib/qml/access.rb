module QML

  # {Access} enables classes to be exposed to QML.
  #
  module Access
    # @!parse include Reactive
    # @!parse extend ClassMethods

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
        include Reactive
        extend ClassMethods
      end
    end

    # allowed name patterns for exposed method names
    ALLOWED_PATTERN = /^[a-zA-Z_]\w*$/

    module ClassMethods

      # Registers the class as a QML type.
      # @param opts [Hash]
      # @option opts [String] :under the namespece which encapsulates the exported QML type. If not specified, automatically inferred from the module nesting of the class.
      # @option opts [String] :version the version of the type. Defaults to VERSION constant of the encapsulating module / class of the class.
      # @option opts [String] :name the name of the type. Defaults to the name of the class.
      def register_to_qml(opts = {})
        QML.on_init do
          register_to_qml_impl(opts)
        end
      end

      def register_to_qml_impl(opts)
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
