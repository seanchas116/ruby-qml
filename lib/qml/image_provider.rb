require 'qml/access'
require 'qml/callback_loop'

module QML
  class ImageProvider

    class Callback
      include QML::Access

      def initialize(provider)
        super()
        @provider = provider
      end

      def request(id)
        @provider.request(id)
      end
    end

    attr_reader :qt_image_provider

    def initialize
      @callback = Callback.new(self)
      @qt_image_provider = Plugins.core.createImageProvider(@callback)
      @qt_image_provider.owned_by_ruby = false
    end

    def request(id)
      fail ::NotImplementedError
    end

    CallbackLoop.instance.start
  end
end
