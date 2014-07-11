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
  application.events_processed.each do
    Dispatcher.instance.run_tasks
  end
end
