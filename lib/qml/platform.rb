module QML

  module Platform
    module_function

    def windows?
      !!(/cygwin|mswin|mingw|bccwin|wince|emx/ =~ RUBY_PLATFORM)
    end

    def mac?
      !!(/darwin/ =~ RUBY_PLATFORM)
    end
  end

end