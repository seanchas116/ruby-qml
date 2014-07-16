module QML
  module Reactive

    class Signal

      class Connection
        def initialize(signal, listener)
          @signal = signal
          @listener = listener
        end

        # Disconnects the connection.
        def disconnect
          @signal.disconnect(@listener)
        end
      end

      attr_reader :arity

      # Initializes the Signal.
      # The signal will be variadic if an empty array is given.
      # @param [Array<#to_sym>, Array<()>] params The parameter names.
      def initialize(*params, variadic: false)
        @listeners = []
        if variadic
          @params = nil
          @arity = -1
        else
          @params = params.map(&:to_sym)
          @arity = params.size
        end
      end

      # Calls every connected procedure with given arguments.
      # Raises an ArgumentError when the arity is wrong.
      # @param args the arguments.
      def emit(*args)
        if @arity >= 0 && args.size != @arity
          fail ::ArgumentError ,"wrong number of arguments for signal (#{args.size} for #{@arity})"
        end
        @listeners.each do |listener|
          listener.call(*args)
        end
      end

      alias_method :call, :emit
      alias_method :[], :emit

      # Returns the format of the parameters in the same format as Proc#parameters.
      # @return [Array<Array<Symbol>>]
      def parameters
        @params ? @params.map { |arg| [:req, arg] } : [[:rest, :args]]
      end

      # Converts the signal into a Proc.
      # @return [Proc]
      def to_proc
        method(:emit).to_proc
      end

      # Returns the number of connections.
      # @return [Integer]
      def connection_count
        @listeners.size
      end

      # Connects a procedure.
      # @yield called when #emit is called.
      # @return [QML::Reactive::Signal::Connection]
      def connect(&listener)
        @listeners << listener
        connected(listener)
        Connection.new(self, listener)
      end

      alias_method :each, :connect

      # Disconnects a procedure.
      # @param listener
      # @return [self]
      def disconnect(listener)
        @listeners.delete(listener)
        disconnected(listener)
        self
      end

      # Creates a transformed version of the signal.
      # @return [QML::Reactive::Signal]
      def map(&block)
        Signals::MapSignal.new(self, block)
      end

      alias_method :collect, :map

      # Creates a filtered version of the signal.
      # @return [QML::Reactive::Signal]
      def select(&block)
        Signals::SelectSignal.new(self, block)
      end

      # Creates a merged signal.
      # @return [QML::Reactive::Signal]
      def merge(*others)
        Signals::MergeSignal.new([self] + others)
      end

      private

        def connected(listener)
        end
    
        def disconnected(listener)
        end
    end
  end
end
