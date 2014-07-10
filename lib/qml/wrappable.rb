module QML
  module Wrappable
    # @abstract
    # @return [QtObjectBase] The created wrapper Qt Object
    def create_wrapper
      fail ::NotImplementedError
    end
  end
end
