require 'qml/plugins'

module QML
  Application = Plugins.core.metaObjects['QApplication'].build_class

  class Application

    def self.instance
      Plugins.core.applicationInstance
    end

    def self.notify_error(error)
      instance.notify_error(error)
    end

    def self.new
      Plugins.core.createApplication(ARGV).tap do |app|
        if block_given?
          yield app
          app.exec
        end
      end
    end

    def initialize
      super()
      @extension = Plugins.core.createApplicationExtension(self)
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

    # Starts the event loop of the application.
    # This method never returns until the application quits.
    def exec
      @extension.exec
    end

    # Called when an Ruby error is occured in executing Qt code.
    # @param error The error (or the exception)
    def notify_error(error)
      warn "-- An error occured when running Ruby code from Qt --"
      warn "#{error.class.name}: #{error.message}"
      warn "Backtrace: \n\t#{error.backtrace.join("\n\t")}"
    end
  end

  def application
    Application.instance
  end
  module_function :application
end
