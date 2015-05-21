module QML
  class JSArray < JSObject
    # @return [Array]
    def to_a
      each.to_a
    end
  end
end
