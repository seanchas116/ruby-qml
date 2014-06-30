require 'qml/plugins'

module QML
  Application = Kernel.application_meta_object.build_class
  Engine = Kernel.engine_meta_object.build_class
  Context = Plugins.core.metaObjects['QQmlContext'].build_class
  Component = Plugins.core.metaObjects['QQmlComponent'].build_class
end
