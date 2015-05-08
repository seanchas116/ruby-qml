
module QML
  class PropertyInfo
    attr_accessor :initializer
  end

  module PropertyDefine
    def self.included(derived)
      derived.extend ClassMethods
    end

    module ClassMethods
      def property(name, &block)
        if property_infos.has_key?(name)
          property_infos[name].initializer = block
          return
        end

        property_infos[name] = PropertyInfo.new.tap do |info|
          info.initializer = block
        end

        name
      end

      def properties
        property_infos.keys
      end

      def property_infos
        if superclass.include? PropertyDefine
          superclass.property_infos.merge(@property_infos)
        else
          @property_infos
        end
      end
    end
  end
end
