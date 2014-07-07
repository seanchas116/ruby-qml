module QML
  module Dispatchable

    class Proxy < BasicObject
      def initialize(obj)
        @obj = obj
      end

      def method_missing(name, *args, &block)
        ::QML.later do
          @obj.__send__(name, *args, &block)
        end
      end
    end

    def later
      Proxy.new(self)
    end
  end
end
