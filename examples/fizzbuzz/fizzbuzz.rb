require 'qml'
require 'pathname'

module Examples
  module FizzBuzz
    VERSION = '0.1.1'

    class FizzBuzz
      include QML::Access
      register_to_qml

      property(:input) { '0' }
      property(:result) { '' }
      signal :inputWasFizzBuzz, []

      on_changed :input do
        i = input.to_i
        self.result = case
        when i % 15 == 0 && i.nonzero?
          inputWasFizzBuzz.emit
          "FizzBuzz"
        when i % 3 == 0 && i.nonzero?
          "Fizz"
        when i % 5 == 0 && i.nonzero?
          "Buzz"
        else
          i.to_s
        end
      end

      def quit
        puts "quitting..."
        QML.application.quit
      end
    end
  end
end

QML.run do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
