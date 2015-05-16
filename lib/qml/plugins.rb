require 'pathname'

module QML
  module Plugins
    class << self
      def rubyqml
        @plugin ||= QML::PluginLoader.new(ROOT_PATH + "ext/qml/rubyqml-plugin", "rubyqml-plugin").instance
      end
    end
  end
end
