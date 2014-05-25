module QML
  class UniqueWrapper

    def self.from_qt(qobj)
      wrapper = qobj.custom_data[:wrapper]
      unless wrapper
        wrapper = wrap(qobj)
      end
      wrapper
    end

    def self.wrap(qobj)
      fail ::NotImplementedError
    end

    def initialize(qobj)
      qobj.custom_data[:wrapper] = self
    end

  end
end
