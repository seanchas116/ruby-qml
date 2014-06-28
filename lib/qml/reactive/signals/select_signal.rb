require 'qml/reactive/chained_signal'

module QML
  module Reactive
    module Signals
      class SelectSignal < ChainedSignal
        def initialize(source, pred)
          super(*source.parameters.map(&:last))
          @source = source
          @pred = pred
        end

        def connect_to_sources
          @source.connect do |*args|
            emit(*args) if @pred.call(*args)
          end
        end
      end
    end
  end
end
