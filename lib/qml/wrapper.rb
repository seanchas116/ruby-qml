module QML
  module Wrapper
    attr_reader :wrapper

    def wrapper_init(qobj, create_wrapper)
      @wrapper = qobj.custom_data[:wrapper] || (qobj.custom_data[:wrapper] = create_wrapper.call(qobj))
    end
  end
end
