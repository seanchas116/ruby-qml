require 'qml/reactive/signal'

module QML
  module Reactive
    class ChainedSignal < Signal
      private
      def connected(listener)
        if connection_count == 1
          @connections = Array(connect_to_sources)
        end
      end

      def disconnected(listener)
        if connection_count == 0
          @connections.each(&:disconnect)
          @connections = nil
        end
      end

      def connect_to_sources
        fail NotImplementedError
      end
    end
  end
end
