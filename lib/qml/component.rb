require 'qml/plugins'
require 'qml/engine'

module QML
  class Component
    include Wrapper

    attr_reader :engine, :data, :path

    def initialize(engine, data: nil, path: nil)
      @engine = engine
      @data = data
      @path = path
      wrapper_init Plugins.core.createComponent(engine.qt_engine), Plugins.core.method(:createComponentWrapper)

      if data
        wrapper.loadString(data, (path && path.to_s) || '')
      elsif path
        wrapper.loadFile(path.to_s)
      else
        fail QMLError, 'neither data nor path specified'
      end
    end

    def create
      wrapper.create
    end

    def qt_component
      wrapper.component
    end
  end
end
