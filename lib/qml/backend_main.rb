require 'qml/c_lib'
require 'singleton'

module QML
  class BackendMain < CLib.rbqml_backendmain_static_metaobject.ruby_class
    include Singleton

    def initialize
      super(CLib.rbqml_backendmain_instance, destroy: false)
    end

    def self.respond_to?(name, include_private = false)
      super || instance.respond_to?(name, include_private)
    end

    def self.method_missing(name, *args, &block)
      super unless instance.respond_to?(name)
      instance.public_send(name, *args, &block)
    end
  end
end
