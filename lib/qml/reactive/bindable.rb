module QML
  module Reactive
    module Bindable

      def initialize(*args, &block)
        super
        @connections = []
      end

      # Sets a new value.
      # The old property binding is discarded.
      # @param new_value new value 
      def value=(new_value, unbind = true)
        self.unbind if unbind
        unless value == new_value
          super(new_value)
          changed.emit(new_value)
        end
        new_value
      end

      alias_method :set_value, :value=

      # @return The property value
      def value
        touch
        super
      end

      # @api private
      module Resolver
        @sources_stack = []
        class << self
          def eval_and_resolve(&block)
            @sources_stack.push([])
            ret = block.call
            [ret, @sources_stack.pop]
          end

          def add(property)
            current = @sources_stack.last
            current && current << property
          end
        end
      end

      # Sets a property binding.
      # The block is re-evaluated when any of other properties used in it is updated
      # and the result is used as the new property value.
      # @yield
      # @example
      #    p1 = Property.new
      #    p2 = Property.new
      #    p1.bind { p2.value + 1 }
      #    p2.value = 10
      #    p1.value #=> 11
      # @return The property value
      def bind(&block)
        unbind
        value, sources = Resolver.eval_and_resolve(&block)
        set_value(value, false)
        @connections = sources.map do |source|
          fail Error, 'Recursive binding' if source == self
          source.changed.connect { set_value(block.call, false) }
        end
      end

      def unbind
        @connections.each(&:disconnect)
      end

      private

      def touch
        Resolver.add(self)
      end
    end
  end
end
