module QML
  class UniqueWrapper

    def self.from_qt(qobj)
      qobj.custom_data[:wrapper] || wrap(qobj)
    end

    def self.wrap(qobj)
      fail ::NotImplementedError
    end

    attr_reader :qt_object

    def initialize(qobj)
      qobj.custom_data[:wrapper] = self
      @qt_object = qobj
    end
  end
end
