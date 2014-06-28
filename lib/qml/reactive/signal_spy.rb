module QML
  module Reactive

    class Signal
      def spy
        SignalSpy.new(self)
      end
    end

    class SignalSpy

      attr_reader :args

      def initialize(signal)
        @args = []
        signal.connect(&method(:on_emitted))
      end

      private

        def on_emitted(*args)
          @args << args
        end
    end

  end
end
