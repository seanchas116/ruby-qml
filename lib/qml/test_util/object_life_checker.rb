require 'qml/plugins'

module QML
  module TestUtil

    ObjectLifeChecker = Plugins.test_util.meta_objects['RubyQml::ObjectLifeChecker'].build_class

    class ObjectLifeChecker

      def self.new(obj)
        Plugins.test_util.create_object_life_checker(obj)
      end
      alias_method :alive?, :is_alive
      alias_method :owned_by_qml?, :is_owned_by_qml
    end
  end
end
