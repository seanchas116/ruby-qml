require 'singleton'
require 'qml/plugins'

=begin
module QML
  class CallbackLoop
    include Singleton

    attr_reader :thread

    def start
      @thread ||= Thread.new do
        begin
          Plugins.core.callback_loop.exec
        rescue => e
          puts e.message
          puts e.backtrace.join("\n")
        end
      end
      at_exit do
        Plugins.core.callback_loop.stop
      end
    end
  end
end
=end
