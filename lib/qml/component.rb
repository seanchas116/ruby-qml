require 'qml/plugins'
require 'qml/engine'

module QML
  Component = Plugins.core.metaObjects['QQmlComponent'].build_class

  class Component
    attr_reader :data, :path

    def self.new(data: nil, path: nil)
      Plugins.core.createComponent(Engine.instance).instance_eval do
        @data = data
        @path = path

        if data
          @extension.loadString(data, (path && path.to_s) || '')
        elsif path
          @extension.loadFile(path.to_s)
        else
          fail QMLError, 'neither data nor path specified'
        end

        self
      end
    end

    def initialize
      super()
      @extension = Plugins.core.createComponentExtension(self)
    end

    def create
      @extension.create
    end
  end
end
