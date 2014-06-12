module QML
  module NameHelper
    module_function

    def to_underscore(sym)
      sym.to_s.gsub(/([A-Z]+)([A-Z][a-z])/,'\1_\2').gsub(/([a-z\d])([A-Z])/,'\1_\2').downcase.to_sym
    end
    def to_upper_underscore(sym)
      to_underscore(sym).upcase
    end
  end
end
