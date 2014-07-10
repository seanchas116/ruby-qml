module QML
  module Wrappable
    # @abstract
    # @return [Wrapper] The created wrapper Qt Object
    def create_wrapper
      fail ::NotImplementedError
    end
  end
end
