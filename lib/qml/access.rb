require 'ostruct'
require 'ropework'
require 'qml/qml'

module QML
  module Access
    def self.included(derived)
      derived.class_eval do
        include Ropework::Object
        include InstanceMethods
        extend ClassMethods
      end
    end

    ALLOWED_PATTERN = /^[a-zA-Z_]\w*$/

    module ClassMethods

      # Registers the class as a QML type.
      # You must call this method in the bottom of class definition
      # because it creates the meta object for the class and it is immutable after creation.
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

        access_support.register_to_qml(under, versions[0], versions[1], name)
      end

      def access_support
        @access_support ||= create_access_support
      end

      private

      def create_access_support
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

        AccessSupport.new(self, classname, method_infos, signal_infos, property_infos)
      end
    end

    module InstanceMethods
      def initialize(*args, &block)
        super
        signal_names = signals + properties.map { |name| :"#{name}_changed" }
        access_support = self.class.access_support
        signal_names.each do |name|
          __send__(name).connect do |*args|
            @access_objects.each do |obj|
              obj.class.meta_object.invoke_method(obj.pointer, name, args)
            end
          end
        end
        @access_objects = []
      end

      attr_reader :access_objects
    end
  end
end
