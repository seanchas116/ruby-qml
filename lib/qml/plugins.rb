require 'qml/plugin_loader'
require 'pathname'

module QML
  module Plugins

    PATH = Pathname(__FILE__) + '../../../ext/qml/plugins'

    class << self
      def core
        @core ||= load('core')
      end

      private

      def load(name)
        QML::PluginLoader.new(PATH + name, "rubyqml-#{name}").instance
      end
    end

  end
end
