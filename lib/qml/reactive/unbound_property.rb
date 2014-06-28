require 'qml/reactive/property'
require 'qml/reactive/unbound_signal'

module QML
  module Reactive
    class UnboundProperty
      attr_reader :name, :initial_value, :initial_binding, :owner, :notifier_signal, :original

      def initialize(name, initial_value, initial_binding, owner, factory = nil)
        @name = name
        @initial_value = initial_value
        @initial_binding = initial_binding
        @owner = owner
        @notifier_signal = UnboundSignal.new(:"#{name}_changed", [:new_value], false, owner)
        @factory = factory
        @original = name
      end

      def alias(name)
        notifier_signal_orig = @notifier_signal
        dup.tap do |property|
          property.instance_eval do
            @name = name
            @notifier_signal = notifier_signal_orig.alias(:"#{name}_changed")
          end
        end
      end

      def alias?
        @original != @name
      end

      def bind(obj)
        return @factory.call(obj, self) if @factory
        Property.new.tap do |p|
          p.value = @initial_value if @initial_value
          p.bind { obj.instance_eval &@initial_binding } if @initial_binding
        end
      end
    end
  end
end
