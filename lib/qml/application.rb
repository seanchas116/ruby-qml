require 'qml/qt_classes'

module QML

  class Application

    def self.instance
      Kernel.application
    end

    def self.notify_error(error)
      instance.notify_error(error)
    end

    def self.new
      fail ApplicationError, "cannot create Application instance manually"
    end

    def initialize
      super()
      @extension = Plugins.core.createApplicationExtension(self)
    end

    def engine
      Kernel.engine
    end

    def context
      engine.context
    end

    def root_component
      @root_component or fail ApplicationError, "QML data or file has not been loaded"
    end

    def load(data: nil, path: nil)
      @root_component = Component.new(data: data, path: path)
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

    def force_deferred_deletes
      @extension.force_deferred_deletes
    end

    def collect_garbage
      ::GC.start
      engine.collect_garbage
      force_deferred_deletes
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
    Application.instance.tap do |app|
      if block_given?
        yield app
        app.exec
      end
    end
  end

  module_function :application
end
