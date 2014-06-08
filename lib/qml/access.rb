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

      def access_support
        @access_composer ||= create_access_support
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

        AccessSupport.new(classname, method_infos, signal_infos, property_infos)
      end
    end

    module InstanceMethods
      def initialize(*args, &block)
        super
        signal_names = signals + properties.map { |name| :"#{name}_changed" }
        signal_names.each do |name|
          public_send(name).connect do |*args|
            self.class.access_support.emit_signal(self, name, args)
          end
        end
      end

      def access_object
        (@_access_object ||= QtObjectPointer.new).tap do |accessobj|
          self.class.access_support.update_access_object(self, accessobj)
        end
      end
    end
  end
end
