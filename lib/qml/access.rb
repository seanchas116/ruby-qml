require 'ostruct'
require 'qml/qml'
require 'qml/dispatchable'

module QML
  module Access
    include Dispatchable

    def self.included(derived)
      derived.class_eval do
        include Reactive::Object
        include SignalInitialization
        extend ClassMethods
      end
    end

    # @api private
    def self.unregistered_classes
      @unregistered_classes ||= []
    end

    # @api private
    def self.register_classes
      unregistered_classes.each(&:register_to_qml_real)
    end

    ALLOWED_PATTERN = /^[a-zA-Z_]\w*$/

    module ClassMethods

      # Registers the class as a QML type.
      # @param under [String|nil] The namespece which encapsulates the exported QML type. If not specified, automatically inferred from the module nesting of the class.
      # @param version [String|nil] The version of the type. Defaults to VERSION constant of the encapsulating module / class of the class.
      # @param name [String|nil] The name of the type. Defaults to the name of the class.
      # @example
      #   class Foo
      #     # ... definitions
      #     register_to_qml
      #   end
      def register_to_qml(under: nil, version: nil, name: nil)
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

        @qml_registeration = {
          under: under,
          version_major: versions[0],
          version_minor: versions[1],
          name: name
        }
        Access.unregistered_classes << self
      end

      # @api private
      def register_to_qml_real
        access_wrapper_factory.register_to_qml(
          @qml_registeration[:under],
          @qml_registeration[:version_major],
          @qml_registeration[:version_minor],
          @qml_registeration[:name])
      end

      # @api private
      def access_wrapper_factory
        @access_wrapper_factory ||= create_access_wrapper_factory
      end

      private

      def create_access_wrapper_factory
        classname = "RubyQml::Access::#{name}"

        signals = instance_signals.grep(ALLOWED_PATTERN)
        properties = instance_properties.grep(ALLOWED_PATTERN)

        signal_infos = signals
          .map { |name| instance_signal(name) }
          .reject(&:variadic?)
          .map { |signal| OpenStruct.new(name: signal.name, params: signal.parameters.map(&:last)) }

        property_infos = properties
          .map { |name| OpenStruct.new(name: name, getter: name, setter: :"#{name}=", notifier: :"#{name}_changed") }

        methods = ancestors.take_while { |k| k.include?(Access) }
          .map { |k| k.instance_methods(false) }.inject(&:|)
          .grep(ALLOWED_PATTERN)
        ignored_methods = signals | property_infos.flat_map { |p| [p.getter, p.setter, p.notifier] }
        method_infos = (methods - ignored_methods)
          .map { |name| instance_method(name) }
          .select { |method| method.parameters.all? { |param| param[0] == :req } }
          .map { |method| OpenStruct.new(name: method.name, params: method.parameters.map(&:last)) }

        AccessWrapperFactory.new(self, classname, method_infos, signal_infos, property_infos)
      end
    end

    module SignalInitialization
      def initialize(*args, &block)
        super
        signal_names = signals + properties.map { |name| :"#{name}_changed" }
        signal_names.each do |name|
          __send__(name).connect do |*args|
            @access_wrappers.each do |obj|
              obj.class.meta_object.invoke_method(obj.pointer, name, args)
            end
          end
        end
      end
    end

    # @api private
    attr_reader :access_wrappers

    def initialize(*args, &block)
      super
      @access_wrappers = []
    end
  end
end
