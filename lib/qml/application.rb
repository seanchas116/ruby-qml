require 'qml/simple_application'

module QML
  class Application < SimpleApplication

    def initialize(qt_app: nil)
      super(qt_app: qt_app)
      if block_given?
        yield self
        exec
      end
    end

    def engine
      @engine ||= Engine.new
    end

    def context
      engine.context
    end

    def root_component
      @root_component or fail "QML data or file has not been loaded"
    end

    def load(data: nil, path: nil)
      @root_component = Component.new(engine, data: data, path: path)
      @root = @root_component.create
    end

    def load_data(data)
      load(data: data)
    end

    def load_path(path)
      load(path: path)
    end

    def root
      @root or fail "QML data or file has not been loaded"
    end
  end
end
