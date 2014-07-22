require 'qml/reactive/unbound_signal'
require 'qml/reactive/unbound_property'

module QML
  module Reactive

    # {Object} is used to define signals and properties within class definition.
    # Signals and properties will be inherited by and can be overridden in subclasses.
    # @note Currently {Object} can be included only by classes, not modules.
    # @see Object::ClassMethods
    module Object

      # When {Object} is included by a class, the class extends {ClassMethods} to add the common class methods.
      def self.included(derived)
        fail Error, "SignalDef must be included in a class" unless derived.is_a? ::Class
        derived.extend(ClassMethods)
      end

      def initialize(*args, &block)
        super
        properties = self.class.instance_property_hash
          .values
          .map { |property| self.class.instance_property(property.original) }
          .uniq
        notifier_signals = properties.map(&:notifier_signal)
        signals = self.class.instance_signal_hash
          .values
          .map { |signal| self.class.instance_signal(signal.original) }
          .uniq - notifier_signals

        properties.each do |property|
          p = property.bind(self)
          instance_variable_set(:"@_property_#{property.name}", p)
          instance_variable_set(:"@_signal_#{property.notifier_signal.name}", p.changed)
        end
        signals.each do |signal|
          instance_variable_set(:"@_signal_#{signal.name}", signal.bind(self))
        end

        self.class.send(:initial_connections_hash).each do |name, blocks|
          blocks.each do |block|
            signal(name).connect do |*args|
              instance_exec(*args, &block)
            end
          end
        end
      end

      # @!method signal(name)
      #   Returns a signal.
      #   @param name [Symbol] The signal name
      #   @return [QML::Reactive::Signal]

      # @!method signals
      #   Returns all signal names.
      #   @return [Array<Symbol>]

      # @!method property(name)
      #   Returns a property.
      #   @param name [Symbol] The property name
      #   @return [QML::Reactive::Property]

      # @!method properties
      #   Returns all property names.
      #   @return [Array<Symbol>]

      [%w{signal signals}, %w{property properties}].each do |singular, plural|
        class_eval <<-EOS, __FILE__, __LINE__ + 1
          def #{singular}(name)
            name = self.class.instance_#{singular}(name).original
            instance_variable_get(:"@_#{singular}_\#{name}") or
              fail ::NameError, "undefined #{singular} '\#{name}' for class '\#{self.class}'"
          end

          def #{plural}
            self.class.instance_#{plural}
          end
        EOS
      end

      # @!parse extend ClassMethods

      module ClassMethods

        # @!method instance_signals(include_super = true)
        #   Returns all signal names for the class.
        #   @param include_super [true|false]
        #   @return [Array<Symbol>]

        # @!method instance_signal(name)
        #   Returns an unbound signal.
        #   @param name [Symbol] The signal name
        #   @return [QML::Reactive::UnboundSignal]

        # @!method instance_properties(include_super = true)
        #   Returns all property names for the class.
        #   @param include_super [true|false]
        #   @return [Array<Symbol>]

        # @!method instance_property(name)
        #   Returns an unbound property.
        #   @param name [Symbol] The property name
        #   @return [QML::Reactive::UnboundProperty]
        [%w{signal signals}, %w{property properties}].each do |singular, plural|
          class_eval <<-EOS, __FILE__, __LINE__ + 1
            def instance_#{singular}_hash(include_super = true)
              if include_super && superclass.include?(Object)
                superclass.instance_#{singular}_hash.merge instance_#{singular}_hash(false)
              else
                @instance_#{singular}_hash ||= {}
              end
            end

            def instance_#{plural}(include_super = true)
              instance_#{singular}_hash(include_super).keys
            end

            def instance_#{singular}(name)
              instance_#{singular}_hash[name] or fail ::NameError, "undefined #{singular} '\#{name}' for class '\#{self}'"
            end
          EOS
        end

        # Defines a signal for the class.
        # The signal will be variadic if args == nil.
        # @param name [#to_sym] The signal name
        # @param params [Array<#to_sym>, nil] The signal parameter names
        # @param opts [Hash]
        # @option opts [Proc] :factory (nil)
        # @return [Symbol] The signal name
        # @example
        #   class Button
        #     include QML::Reactive::Object
        #     signal :pressed, [:pos]
        #     def press(pos)
        #       pressed.emit(pos)
        #     end
        #   end
        #
        #   button = Button.new
        #   button.pressed.connect { |pos| puts "Pressed at #{pos}" }
        #   button.press([10, 20])
        #
        #   class ColorButton < Button
        #     signal :pressed, [:pos, :color]
        #   end
        #
        #   color_button = ColorButton.new
        #   color_button.pressed.connect { |pos, color| "#{color} button pressed at #{pos}" }
        #   color_button.press([10, 20], 'red')
        def signal(name, params, opts = {})
          name.to_sym.tap do |name|
            params = params.map(&:to_sym)
            add_signal(UnboundSignal.new(name, params, false, self, opts[:factory]))
          end
        end

        # Defines a variadic signal.
        # Variadic signals do not restrict the number of arguments.
        # @param opts [Hash]
        # @option opts [Proc] :factory (nil)
        # @see #signal
        def variadic_signal(name, opts = {})
          name.to_sym.tap do |name|
            add_signal(UnboundSignal.new(name, nil, true, self, opts[:factory]))
          end
        end

        # Aliases a signal.
        # @return [Symbol] The new name
        def alias_signal(name, original_name)
          add_signal(instance_signal(original_name).alias(name))
          name
        end

        # Defines a property for the class.
        # @param name [#to_sym] The name of the property
        # @param init_value The initial value (optional)
        # @param opts [Hash]
        # @option opts [Proc] :factory (nil)
        # @yield The initial property binding (optional)
        # @return [Symbol] The name
        # @example
        #   class Foo
        #     include QML::Reactive::Object
        #     property(:name) { 'hogehoge' }
        #     ...
        #   end
        #   Foo.new.name #=> 'hogehoge'
        #   Foo.new.name = 'foobar'
        #   Foo.new.name #=> 'foobar'
        #   Foo.new.name_changed.connect do |new_name|
        #     ...
        #
        #   class Bar < Foo
        #     property(:name) { 'piyopiyo' }
        #   end
        #   Bar.new.name #=> 'piyopiyo'
        def property(name, init_value = nil, opts = {}, &init_binding)
          name = name.to_sym
          add_property(UnboundProperty.new(name, init_value, init_binding, self, opts[:factory]))
          name
        end

        # Aliases a property.
        # @return [Symbol] The new name
        def alias_property(name, original_name)
          add_property(instance_property(original_name).alias(name))
          name
        end

        # Adds a signal handler.
        # @param signal_name The name of the signal
        # @yield The block that is connected to the signal during object initialization
        def on(signal_name, &block)
          # just for check
          instance_signal(signal_name)
          @initial_connections_hash ||= {}
          @initial_connections_hash[signal_name] ||= []
          @initial_connections_hash[signal_name] << block
        end

        # Adds a handler to a property change signal.
        # @param property_name The name of the property
        # @yield The block that is connected to the property change signal during object initialization
        # @example
        #   class Foo
        #     property :bar, ''
        #     on_changed :bar do
        #       some_action
        #     end
        #     def some_action
        #       ...
        #     end
        #   end
        # @see #on
        def on_changed(property_name, &block)
          on(:"#{property_name}_changed", &block)
        end

        private

        def add_signal(signal)
          instance_signal_hash(false)[signal.name] = signal

          class_eval <<-EOS, __FILE__, __LINE__ + 1
            def #{signal.name}
              @_signal_#{signal.original}
            end
          EOS
        end

        def add_property(property)
          instance_property_hash(false)[property.name] = property

          class_eval <<-EOS, __FILE__, __LINE__ + 1
            def #{property.name}(&block)
              @_property_#{property.original}.value(&block)
            end
            def #{property.name}=(new_value)
              @_property_#{property.original}.value = new_value
            end
          EOS

          add_signal(property.notifier_signal)
        end

        def initial_connections_hash(include_super = true)
          if include_super && superclass.include?(Object)
            superclass.send(:initial_connections_hash).dup.tap do |hash|
              initial_connections_hash(false).each do |key, blocks|
                hash[key] ||= []
                hash[key] += blocks
              end
            end
          else
            @initial_connections_hash ||= {}
          end
        end
      end
    end
  end
end

