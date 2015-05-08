module QML
  class JSObject
    def keys
      each.map { |k, v| k }
    end

    def values
      each.map { |k, v| v }
    end

    def respond_to?(method)
      # TODO
    end

    def method_missing(method, *args)
      unless has_key?(method)
        fail NoMethodError, "property `#{method}` not found in #{self}"
      end

      prop = self[method]
      if prop.is_a? JSFunction
        prop.call_with_instance(self, *args)
      else
        prop
      end
    end
  end
end
