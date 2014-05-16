require 'singleton'
require 'qml/class_builder'

module QML

  class ClassProvider
    include Singleton

    def initialize
      @classes = {}
    end

    def update_class(metaobj)
      klass = @classes[metaobj.name]
      builder = ClassBuilder.new(metaobj)

      if klass
        builder.update(klass)
      else
        klass = builder.create
        @classes[metaobj.name] = klass
      end
      klass
    end
  end
end
