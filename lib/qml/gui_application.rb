require 'qml/plugins'
require 'qml/unique_wrapper'

module QML
  class GUIApplication < UniqueWrapper

    def self.wrap(qt_app)
      new(qt_app: qt_app)
    end

    def initialize(qt_app: nil)
      qt_app ||= Plugins.core.createGuiApplication(ARGV)
      super(qt_app)
      @extension = Plugins.core.createApplicationExtension(qt_app)
    end

    def qt_application
      @extension.application
    end
  end
end
