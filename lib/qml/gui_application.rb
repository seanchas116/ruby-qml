require 'qml/plugins'

module QML
  class GUIApplication

    attr_reader :wrapper

    def initialize
      app = Plugins.core.createGuiApplication(ARGV)
      @wrapper = Plugins.core.createApplicationWrapper(app)
    end
  end
end
