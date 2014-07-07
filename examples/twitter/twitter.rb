$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'twitter'
require 'yaml'

module Examples
  module Twitter
    VERSION = '0.1'

    class TwitterModel < QML::Data::ArrayModel
      column :tweet_text, :user_name, :user_icon
    end

    class TwitterController
      include QML::Access
      register_to_qml

      property :model, TwitterModel.new

      def initialize
        super()

        streaming_client = ::Twitter::Streaming::Client.new do |config|
          data = YAML.load((Pathname(__FILE__) + '../config.yml').read)
          %w{consumer_key consumer_secret access_token access_token_secret}.each do |key|
            config.public_send("#{key}=", data[key])
          end
        end

        Thread.new do
          begin
            streaming_client.user do |object|
              case object
              when ::Twitter::Tweet
                later.add_tweet(object)
              end
            end
          rescue => e
            puts e.message
          end
        end
      end

      def add_tweet(tweet)
        hash = {tweet_text: tweet.text, user_name: tweet.user.name, user_icon: tweet.user.profile_image_uri.to_s}
        puts hash
        model.unshift hash
      end
    end
  end
end

QML.application do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
