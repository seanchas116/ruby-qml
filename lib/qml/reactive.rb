module QML

  module Reactive
    # @!parse extend ClassMethods

    # @api private
    class PropertyInfo
      attr_accessor :initializer
    end

    # @api private
    class SignalInfo
      attr_accessor :params
      attr_accessor :listeners

      def initialize
        @listeners = []
      end
    end

    def initialize(*args, &block)

      self.class.property_infos.each do |name, info|
        if info.initializer
          self.__send__ :"#{name}=", instance_eval(&info.initializer)
        end
      end

      self.class.signal_infos.each do |name, info|
        __send__(name).connect do |*args|
          info.listeners.each do |listener|
            instance_exec(*args, &listener)
          end
        end
      end

      super
    end

    def self.included(derived)
      derived.class_eval do
        extend ClassMethods
      end
    end

    module ClassMethods

      def signal(name, params = [])
        name = name.to_sym
        signal_infos(false)[name] = SignalInfo.new.tap do |info|
          info.params = params.map(&:to_sym)
        end

        class_eval <<-EOS, __FILE__, __LINE__ + 1
          def #{name}
            @_signal_#{name} ||= begin
              args = self.class.signal_infos[:#{name}].params
              Signal.new(args)
            end
          end
        EOS

        name
      end

      private :signal

      def signals(include_super = true)
        signal_infos(include_super).keys
      end

      def signal_infos(include_super = true)
        if include_super && superclass.include?(Access)
          superclass.signal_infos.merge(signal_infos(false))
        else
          @signal_infos ||= {}
        end
      end

      def property(name, &block)
        name = name.to_sym
        signal(:"#{name}_changed", [:"new_#{name}"])

        property_infos(false)[name] = PropertyInfo.new.tap do |info|
          info.initializer = block
        end

        class_eval <<-EOS, __FILE__, __LINE__ + 1
          attr_reader :#{name}
          def #{name}=(new_value)
            new_value = new_value
            if @#{name} != new_value
              @#{name} = new_value
              #{name}_changed.emit(new_value)
            end
          end
        EOS

        name
      end

      private :property

      def properties(include_super = true)
        property_infos(include_super).keys
      end

      def property_infos(include_super = true)
        if include_super && superclass.include?(Access)
          superclass.property_infos.merge(property_infos(false))
        else
          @property_infos ||= {}
        end
      end

      def on(signal, &block)
        info = signal_infos(false)[signal.to_sym] or fail AccessError, "no signal `#{signal}` found"
        info.listeners << block
        block
      end

      def on_changed(property, &block)
        on(:"#{property}_changed", &block)
      end

      private :on, :on_changed

    end

  end

end
