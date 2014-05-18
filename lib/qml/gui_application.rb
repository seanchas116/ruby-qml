require 'qml/plugins'

module QML
  class GUIApplication
    include Wrapper

    def initialize
      wrapper_init(Plugins.core.createGuiApplication(ARGV),
                   Plugins.core.method(:createApplicationWrapper))
    end

    def qt_application
      wrapper.application
    end
  end
end
