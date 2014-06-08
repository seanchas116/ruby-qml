require 'qml/plugins'
require 'qml/engine'

module QML
  class Component
    attr_reader :engine, :data, :path

    def initialize(engine, data: nil, path: nil)
      @engine = engine
      @data = data
      @path = path
      @extension = Plugins.core.createComponentExtension(Plugins.core.createComponent(engine.qt_object))

      if data
        @extension.loadString(data, (path && path.to_s) || '')
      elsif path
        @extension.loadFile(path.to_s)
      else
        fail QMLError, 'neither data nor path specified'
      end
    end

    def create
      @extension.create
    end
  end
end
