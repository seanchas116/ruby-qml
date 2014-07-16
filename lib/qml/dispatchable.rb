module QML

  # {Dispatchable} provides a handy way for asynchronous (queued) method calls
  # which actually processed later in the event loop.
  # @see QML.later
  module Dispatchable

    class Proxy < BasicObject
      def initialize(obj)
        @obj = obj
      end

      # Actually enqueues the method call using {QML.later}.
      def method_missing(name, *args, &block)
        ::QML.later do
          @obj.__send__(name, *args, &block)
        end
      end
    end

    # Returns a proxy object for calling a method asynchronously within the event loop.
    # @return [Proxy]
    # @example
    #   def on_button_clicked
    #     Thread.new do
    #       result = do_task
    #       later.set_result_to_ui(result)
    #     end
    #   end
    def later
      Proxy.new(self)
    end
  end
end
