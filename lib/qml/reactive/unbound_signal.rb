require 'qml/reactive/signal'

module QML
  module Reactive
    class UnboundSignal
      attr_reader :name, :owner, :original

      def initialize(name, params, variadic, owner, factory = nil)
        @name = name
        @params = params
        @variadic = variadic
        @owner = owner
        @factory = factory
        @original = name
      end

      def alias(name)
        dup.tap do |signal|
          signal.instance_eval do
            @name = name
          end
        end
      end

      def variadic?
        @variadic
      end

      def alias?
        @original != @name
      end

      def arity
        @params ? @params.size : -1
      end

      def parameters
        @params ? @params.map { |arg| [:req, arg] } : [[:rest, :args]]
      end

      def bind(obj)
        return obj.instance_exec(&@factory) if @factory
        if @variadic
          Signal.new(nil)
        else
          Signal.new(@params)
        end
      end
    end
  end
end
