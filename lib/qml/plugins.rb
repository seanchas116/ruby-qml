require 'qml/plugin_loader'
require 'qml/root_path'
require 'pathname'

module QML
  module Plugins

    PATH = ROOT_PATH + 'ext/qml/plugins'

    class << self
      def core
        @core ||= load('core')
      end

      def test_util
        @test_util ||= load('testutil')
      end

      private

      def load(name)
        QML::PluginLoader.new(PATH + name, "rubyqml-#{name}").instance
      end
    end
  end
end
