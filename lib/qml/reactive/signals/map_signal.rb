require 'qml/reactive/chained_signal'

module QML
  module Reactive
    module Signals
      class MapSignal < ChainedSignal
        def initialize(source, func)
          super(:mapped)
          @source = source
          @func = func
        end

        def connect_to_sources
          @source.connect do |*args|
            emit(@func[*args])
          end
        end
      end
    end
  end
end
