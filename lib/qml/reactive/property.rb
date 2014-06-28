require 'qml/reactive/error'
require 'qml/reactive/simple_property'
require 'qml/reactive/bindable'

module QML
  module Reactive
    class Property < SimpleProperty
      include Bindable

      # @param initial_value The initial value
      # @param holder A custom value holder
      # @yield The initial property binding (optional)
      def initialize(initial_value = nil, &block)
        super()
        self.value = initial_value if initial_value
        bind(&block) if block
      end
    end
  end
end
