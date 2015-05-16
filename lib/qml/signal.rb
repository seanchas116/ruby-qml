module QML

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
    # @param [Array<#to_sym>, nil] params the parameter names (the signal will be variadic if nil).
    def initialize(params)
      @listeners = []
      @params = params.map(&:to_sym)
      @arity = params.size
    end

    # Calls every connected procedure with given arguments.
    # Raises an ArgumentError when the arity is wrong.
    # @param args the arguments.
    def emit(*args)
      if args.size != @arity
        fail ::ArgumentError ,"wrong number of arguments for signal (#{args.size} for #{@arity})"
      end
      @listeners.each do |listener|
        listener.call(*args)
      end
    end

    # Returns the format of the parameters in the same format as Proc#parameters.
    # @return [Array<Array<Symbol>>]
    def parameters
      @params ? @params.map { |arg| [:req, arg] } : [[:rest, :args]]
    end

    # Returns the number of connections.
    # @return [Integer]
    def connection_count
      @listeners.size
    end

    # Connects a procedure.
    # @yield called when #emit is called.
    # @return [QML::Signal::Connection]
    def connect(&listener)
      @listeners << listener
      Connection.new(self, listener)
    end

    # Disconnects a procedure.
    # @param listener
    # @return [self]
    def disconnect(listener)
      @listeners.delete(listener)
      self
    end
  end
end
