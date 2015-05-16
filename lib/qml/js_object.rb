module QML
  class JSObject

    def keys
      each.map { |k, v| k }
    end

    def values
      each.map { |k, v| v }
    end

    def to_hash
      {}.tap do |hash|
        each do |k, v|
          hash[k] =v
        end
      end
    end

    def to_time
      Time.at(getTime.to_i / 1000r).getlocal(-getTimezoneOffset * 60)
    end

    def to_error
      QMLError.new(self['message'])
    end

    def respond_to?(method)
      has_key?(method) || super
    end

    def method_missing(method, *args)
      if method[-1] == '='
        # setter
        key = method.slice(0...-1).to_sym

        unless has_key?(key)
          super
        end
        self[key] = args[0]
      else
        unless has_key?(method)
          super
        end

        prop = self[method]
        if prop.is_a? JSFunction
          prop.call_with_instance(self, *args)
        else
          prop
        end
      end
    end

    def to_qml
      self
    end
  end
end
