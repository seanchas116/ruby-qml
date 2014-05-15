require 'qml/plugins'

module QML
  class Engine

    attr_reader :wrapper

    def initialize
      engine = Plugins.core.createEngine
      @wrapper = Plugins.core.createEngineWrapper(engine)
    end
  end
end
