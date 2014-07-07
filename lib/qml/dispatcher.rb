require 'singleton'
require 'monitor'

module QML

  class Dispatcher
    include Singleton
    include MonitorMixin

    MAX_DURATION = 1/10.to_r

    def initialize
      @tasks = []
      super
    end

    def add_task(&task)
      synchronize do
        @tasks << task
      end
    end

    def empty?
      synchronize do
        @tasks.empty?
      end
    end

    def run_tasks
      synchronize do
        start_time = Time.now
        loop do
          break if @tasks.empty?
          break if start_time.to_r - Time.now.to_r > MAX_DURATION
          task = @tasks.shift
          task.call
        end
      end
    end
  end

  def later(&block)
    Dispatcher.instance.add_task(&block)
  end
  module_function :later
end
