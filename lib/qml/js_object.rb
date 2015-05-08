module QML
  class JSObject
    def respond_to?(method)
      # TODO
    end

    def method_missing(method, *args)
      prop = self[method]
      if prop.is_a? JSFunction
        prop.call(*args)
      else
        prop
      end
    end
  end
end
