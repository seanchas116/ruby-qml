require 'qml/plugins'
require 'qml/wrapper'

module QML
  class Engine
    include Wrapper

    def initialize(qt_engine: nil)
      wrapper_init(qt_engine || Plugins.core.createEngine, Plugins.core.method(:createEngineWrapper))
    end

    def qt_engine
      wrapper.engine
    end
  end
end
