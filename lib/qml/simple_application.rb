require 'qml/plugins'
require 'qml/unique_wrapper'

module QML
  class SimpleApplication < UniqueWrapper

    def self.wrap(qt_app)
      new(qt_app: qt_app)
    end

    def self.instance
      from_qt(Plugins.core.applicationInstance)
    end

    def self.notify_error(error)
      instance.notify_error(error)
    end

    def initialize(qt_app: nil)
      qt_app ||= Plugins.core.createApplication(ARGV)
      super(qt_app)
      @extension = Plugins.core.createApplicationExtension(qt_app)
    end

    # Starts the event loop of the application.
    # This method never returns until the application quits.
    def exec
      @extension.exec
    end

    # Quits the application.
    def quit
      qt_object.quit
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
    SimpleApplication.instance
  end
  module_function :application
end
