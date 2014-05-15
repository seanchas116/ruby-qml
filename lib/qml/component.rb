require 'qml/plugins'
require 'qml/engine'

module QML
  class Component

    attr_reader :wrapper

    def initialize(engine)
      component = Plugins.core.createComponent(engine.wrapper.engine)
      @wrapper = Plugins.core.createComponentWrapper(component)
    end

    def load_str(str, filepath = '')
      @wrapper.loadString(str, filepath.to_s)
      self
    end

    def load_file(filepath)
      @wrapper.loadFile(filepath.to_s)
      self
    end

    def create
      @wrapper.create
    end
  end
end
