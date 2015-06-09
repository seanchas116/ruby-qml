module QML
  # The JSObject represents JavaScript objects.
  class JSObject

    alias_method :each, :each_pair

    # @return [Array<String>]
    def keys
      each.map { |k, v| k }
    end

    # @return [Array]
    def values
      each.map { |k, v| v }
    end

    # @return [Hash]
    def to_hash
      {}.tap do |hash|
        each do |k, v|
          hash[k] =v
        end
      end
    end

    # @return [Time]
    def to_time
      Time.at(getTime.to_i / 1000r).getlocal(-getTimezoneOffset * 60)
    end

    # @return [QML::QMLError]
    def to_error
      QMLError.new(self['message'])
    end

    def respond_to?(method)
      has_key?(method) || super
    end

    # Gets or sets a JS property, or call it as a method if it is a function.
    def method_missing(method, *args, &block)
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
          prop.call_with_instance(self, *args, &block)
        else
          prop
        end
      end
    end

    # @return [QML::JSObject] self
    def to_qml
      self
    end
  end
end
