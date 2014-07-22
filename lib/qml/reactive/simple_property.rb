require 'qml/reactive/signal'

module QML
  module Reactive
    class SimpleProperty
      attr_accessor :value

      # @!attribute [r] changed
      #   @return [QML::Reactive::Signal] The notification signal that is emitted when the value is changed
      attr_reader :changed

      def initialize
        @changed = Signal.new([:new_value])
      end
    end
  end
end
