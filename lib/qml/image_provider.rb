require 'qml/access'
require 'qml/callback_loop'

module QML

  # {ImageProvider} is used to provide images manually to QML.
  #
  # QML requests images to ImageProviders when you use "image:" URL scheme
  # (like "image://my_image_provider/foo.png") in source specification of Image elements.
  #
  # @example
  #   class MyImageProvider < QML::ImageProvider
  #     def request(req)
  #       Thread.new do
  #         image_data = load_image_data()
  #         req.finish(image_data)
  #       end
  #     end
  #   end
  #   QML.engine.add_image_provider(MyImageProvider.new)
  #
  # @see Engine#add_image_provider
  # @see http://qt-project.org/doc/qt-5/qquickimageprovider.html QQuickImageProvider (C++)
  # @see http://qt-project.org/doc/qt-5/qml-qtquick-image.html Image (QML)

  class ImageProvider

    # {Request} represents a image request from QML.
    # @see ImageProvider#request
    class Request
      # The image ID.
      # If you requested "image://my_image_provider/foo/bar.png", the ID would be "foo/bar.png".
      attr_reader :id

      # Finishes the image request.
      # @param data [String] The image file data
      def finish(data)
        fail ::TypeError, "data must be a String" unless data.is_a? ::String
        QML.later do
          @promise.set_data(data)
        end
      end

      private

      def initialize(id, promise)
        @id = id;
        @promise = promise
      end
    end

    attr_reader :qt_image_provider

    def initialize
      @callback = Callback.new(self)
      @qt_image_provider = Plugins.core.createImageProvider(@callback)
      @qt_image_provider.prefer_managed false
    end

    # @abstract
    # @param req [Request] 
    #
    # Implement this method to handle image requests from QML.
    #
    # Call {Request#finish} to finish image loading.
    # Image loading should be done asynchnorously because this method is called within the event loop.

    def request(req)
      fail ::NotImplementedError
    end

    private

    class Callback
      include QML::Access

      def initialize(provider)
        super()
        @provider = provider
      end

      def request(id, promise)
        @provider.request(Request.new(id, promise))
      end
    end
  end
end
