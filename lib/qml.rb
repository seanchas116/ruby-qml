require 'qml/version'
require 'qml/qml'

require 'qml/geometry'
require 'qml/reactive'

require 'qml/meta_object'
require 'qml/plugin_loader'
require 'qml/component'
require 'qml/engine'
require 'qml/context'
require 'qml/application'
require 'qml/image_provider'
require 'qml/qt'
require 'qml/access'
require 'qml/root_path'
require 'qml/dispatcher'
require 'qml/dispatchable'

require 'qml/data'
require 'qml/test_util'

module QML

  def initialized?
    Kernel.initialized?
  end

  # Initializes ruby-qml.
  # @param [Boolean] offscreen set this to true to run application offscreen (without GUI)
  def init(offscreen: false)
    fail AlreadyInitializedError, "ruby-qml already initialized" if initialized?
    argv = [$PROGRAM_NAME]
    argv += %w{-platform offscreen} if offscreen
    Kernel.init(argv)
    application.events_processed.each do
      Dispatcher.instance.run_tasks
    end
  end
  module_function :initialized?, :init
end
