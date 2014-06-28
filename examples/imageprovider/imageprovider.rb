$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'open-uri'
require 'singleton'
require 'celluloid'

module Examples
  module ImageProvider

    VERSION = '0.1.0'

    class ImageFetcher
      include Celluloid
      include Singleton

      def set_url(url)
        @url = url
      end

      def fetch
        open(@url, 'rb') { |f| f.read } rescue nil
      end
    end

    class Controller
      include QML::Access

      property :url, ''

      on_changed :url do
        ImageFetcher.instance.async.set_url url
      end

      register_to_qml
    end

    class Provider < QML::ImageProvider

      def request(id)
        case id
        when 'image'
          ImageFetcher.instance.fetch
        end
      end
    end
  end
end

QML.application do |app|
  app.engine.add_image_provider 'example', Examples::ImageProvider::Provider.new
  app.load_path Pathname(__FILE__) + '../main.qml'
end
