require 'qml/dispatchable'
require 'qml/name_helper'

module QML

  class QtPropertyBase
    attr_reader :changed

    def initialize(metaobj, objptr, name)
      super()
      @metaobj = metaobj
      @objptr = objptr
      @name = name
      @changed = QtSignal.new(metaobj, objptr, @metaobj.notify_signal(@name))
    end

    def value=(newval)
      @metaobj.set_property(@objptr, @name, newval)
    end

    def value
      @metaobj.get_property(@objptr, @name)
    end
  end

  class QtProperty < QtPropertyBase
    include Reactive::Bindable
  end

  class QtSignal < Reactive::Signal
    def initialize(metaobj, objptr, name)
      super(nil)
      @objptr = objptr
      @metaobj = metaobj
      @name = name
      @initialized = false
    end

    private

    def connected(block)
      return if @initialized
      @metaobj.connect_signal(@objptr, @name, method(:emit))
      @initialized = true
    end
  end

  # {QtObjectBase} is the base class for Qt object wrappers.
  #
  # In ruby-qml you can access the followings of Qt objects in Ruby.
  #
  # * Properties as {Reactive::Property}
  # * Signals as {Reactive::Signal}
  # * Q_INVOKABLE methods, slots and QML methods as Ruby methods
  #
  # Properties and signals support is provided by {Reactive::Object}.
  #
  # While their names are camelCase in Qt, ruby-qml aliases them as underscore_case.
  #
  # @example
  #   # QML::Application is actually a wrapper for QApplication
  #   app = QML.application
  #
  #   # set property
  #   app.applicationName = "Test"
  #   app.application_name = "Test" # aliased version
  #
  #   # connect to signal
  #   app.aboutToQuit.connect do # "about_to_quit" is also OK
  #     puts "quitting..."
  #   end
  #
  #   # call slot
  #   app.quit
  class QtObjectBase
    include Dispatchable
    include Reactive::Object

    # @api private
    class SubclassBuilder

      attr_reader :subclass

      def initialize(metaobj, klass)
        @metaobj = metaobj
        @subclass = klass
      end

      def build
        create unless @subclass
        return if @subclass.meta_object == @metaobj

        @metaobj.method_names.reject { |name| @metaobj.signal?(name) }.each do |name|
          define_method(name)
        end
        @metaobj.method_names.select { |name| @metaobj.signal?(name) }.each do |name|
          define_signal(name)
        end
        @metaobj.property_names.each do |name|
          define_property(name)
        end
        @metaobj.enumerators.each do |k, v|
          define_enum(k, v)
        end
        @subclass.__send__ :meta_object=, @metaobj

        self
      end

      private

      def create
        super_metaobj = @metaobj.super_class
        @subclass = Class.new(super_metaobj ? super_metaobj.build_class : QtObjectBase)
      end

      def define_method(name)
        metaobj = @metaobj
        return if metaobj.private?(name)
        underscore = NameHelper.to_underscore(name)

        @subclass.class_eval do
          define_method name do |*args|
            metaobj.invoke_method(@pointer, name, args)
          end
          private name if metaobj.protected?(name)
          alias_method underscore, name unless underscore == name
        end
      end

      def define_signal(name)
        metaobj = @metaobj
        underscore = NameHelper.to_underscore(name)
        @subclass.class_eval do
          variadic_signal name, signal: -> { QtSignal.new(metaobj, @pointer, name) }
          alias_signal underscore, name unless underscore == name
        end
      end

      def define_property(name)
        metaobj = @metaobj
        underscore = NameHelper.to_underscore(name)
        @subclass.class_eval do
          property name, nil, property: -> { QtProperty.new(metaobj, @pointer, name) }
          alias_property underscore, name unless underscore == name
        end
      end

      def define_enum(name, hash)
        define_const(name, hash.values.sort)
        hash.each do |k, v|
          define_const(k, v)
        end
      end

      def define_const(name, value)
        name = (name[0].capitalize + name[1..-1]).to_sym
        underscore = NameHelper.to_upper_underscore(name)
        @subclass.class_eval do
          const_set name, value
          const_set underscore, value unless underscore == name
        end
      end
    end

    class << self
      attr_accessor :meta_object
      private :meta_object=
    end

    attr_accessor :pointer
    private :pointer=

    # Evaluates a JavaScript expression on the QML context of the object.
    # Fails with {QMLError} when the object is not created by QML and does not belong to any context.
    # @param [String] str
    # @return the result.
    # @example
    #   component = QML::Component.new data: <<-EOS
    #     import QtQuick 2.0
    #     QtObject {
    #       property string foo: 'foo'
    #       property string bar: 'bar'
    #     }
    #   EOS
    #   obj = component.create
    #   obj.qml_eval("foo + bar") #=> "foobar"
    def qml_eval(str)
      context = Context.for_object(self)
      fail QMLError, 'belongs to no context' unless context
      context.eval(self, str)
    end

    def inspect
      klass = self.class
      property_inspect = klass.instance_properties.sort
        .reject { |name| klass.instance_property(name).alias? }
        .map do |name|
          "#{name}=" +
            begin
              property(name).value.inspect
            rescue ConversionError
              "<unsupported type>"
            end
        end
        .join(' ')
      classname = klass.name || "[class for #{klass.meta_object.name}]"
      "#<#{classname}:#{__id__} #{property_inspect}>"
    end

    alias_method :to_s, :inspect

    # @return whether the object is managed by Ruby and QML and garbage collected when no longer used.
    def managed?
      pointer.managed?
    end

    # Sets the management status of the object.
    # @param [Boolean] managed
    # @return [Boolean]
    def managed=(managed)
      pointer.managed = managed
    end

    # Sets the management status of the object in safer way.
    # Objects that are created by QML will remain managed and objects that have parents will remain unmanaged.
    # @param [Boolean] managed
    # @return [Boolean]
    def prefer_managed(managed)
      pointer.prefer_managed managed
    end
  end
end
