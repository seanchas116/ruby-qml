module QML
  module JSUtil

    def self.data
      @data ||= QML.engine.evaluate <<-JS
        ({
          classes: {
            Date: Date
          }
        })
      JS
    end

    def self.classes
      data.classes
    end
  end
end
