$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'pathname'

module Examples
  module FizzBuzz
    VERSION = '0.1'

    class FizzBuzz
      include QML::Access

      property :input, '0'
      property :result , ''
      signal :inputWasFizzBuzz, []

      on_changed :input do
        i = input.to_i
        self.result = case
        when i % 15 == 0
          inputWasFizzBuzz.emit
          "FizzBuzz"
        when i % 3 == 0
          "Fizz"
        when i % 5 == 0
          "Buzz"
        else
          i.to_s
        end
      end

      def quit
        puts "quitting..."
        QML.application.quit
      end

      register_to_qml
    end
  end
end

QML.run do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
