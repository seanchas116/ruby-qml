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

    def initialize(qt_app: nil)
      qt_app ||= Plugins.core.createApplication(ARGV)
      super(qt_app)
      @extension = Plugins.core.createApplicationExtension(qt_app)
    end

    def exec
      @extension.exec
    end
  end
end
