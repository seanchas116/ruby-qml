require 'ropework'

module QML
  module Access
    def self.included(derived)
      derived.class_eval do
        include Ropework::PropertyDef
        include Ropework::SignalDef
      end
    end
  end
end
