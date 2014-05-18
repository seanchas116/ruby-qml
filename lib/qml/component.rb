require 'qml/plugins'
require 'qml/engine'

module QML
  class Component
    include Wrapper

    def initialize(engine)
      wrapper_init Plugins.core.createComponent(engine.qt_engine), Plugins.core.method(:createComponentWrapper)
    end

    def load_str(str, filepath = '')
      wrapper.loadString(str, filepath.to_s)
      self
    end

    def load_file(filepath)
      wrapper.loadFile(filepath.to_s)
      self
    end

    def create
      wrapper.create
    end

    def qt_component
      wrapper.component
    end
  end
end
