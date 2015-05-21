$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'twitter'
require 'yaml'

module Examples
  module Twitter
    VERSION = '0.1'

    class TweetFetcher
      def initialize
        config_data = YAML.load((Pathname(__FILE__) + '../config.yml').read)
        config_keys = %w{consumer_key consumer_secret access_token access_token_secret}

        @rest_client = ::Twitter::REST::Client.new do |config|
          config_keys.each do |key|
            config.public_send("#{key}=", config_data[key])
          end
        end
        @streaming_client = ::Twitter::Streaming::Client.new do |config|
          config_keys.each do |key|
            config.public_send("#{key}=", config_data[key])
          end
        end
      end

      def start(word)
        puts "word = #{word}"
        @rest_client.search(word).take(10).each do |t|
          yield t
        end
        @streaming_client.filter(track: word) do |object|
          case object
          when ::Twitter::Tweet
            yield object
          end
        end
      end
    end

    class TwitterController
      include QML::Access
      register_to_qml

      property(:model) { QML::ArrayModel.new(:tweet_text, :user_name, :user_icon) }
      property :word

      def initialize
        super()
      end

      def add_tweet(tweet)
        hash = {tweet_text: tweet.text, user_name: tweet.user.name, user_icon: tweet.user.profile_image_uri.to_s}
        puts hash
        model.unshift hash
      end

      def fetch_tweets
        model.clear
        if @thread
          @thread.kill
        end
        word = self.word
        @thread = Thread.new do
          TweetFetcher.new.start(word) do |tweet|
            QML.next_tick do
              add_tweet(tweet)
            end
          end
        end
        nil
      end
    end
  end
end

QML.run do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
