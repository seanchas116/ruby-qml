require 'qml/reactive/chained_signal'

module QML
  module Reactive
    module Signals
      class MergeSignal < ChainedSignal
        def initialize(sources)
          param_count = sources.map(&:arity).max
          super(*param_count.times.map { |i| :"arg#{i}" })
          @sources = sources
        end

        def connect_to_sources
          @sources.each do |source|
            source.connect(&method(:emit))
          end
        end
      end
    end
  end
end
