
module QML

  class MetaType
    attr_reader :id

    def initialize(id)
      @id = id
    end

    def name
      @name ||= CLib.qmetatype_name(@id).to_sym
    end

    def ==(other)
      id == other.id
    end

    alias_method :eql?, :==

    def hash
      id.hash
    end

    def to_s
      name.to_s
    end

    def inspect
      "<QML::MetaType:#{name}>"
    end

    def ruby_class
      case self
      when BOOL
        [TrueClass, FalseClass]
      when INT
        Integer
      when DOUBLE
        Float
      when Q_STRING
        String
      when Q_VARIANT_LIST
        Array
      when Q_VARIANT_HASH, Q_VARIANT_MAP
        Hash
      when Q_DATE_TIME
        Time
      when Q_VARIANT
        Object
      else
        nil
      end
    end

    VOID = new(43)
    BOOL = new(1)
    INT = new(2)
    DOUBLE = new(6)
    Q_STRING = new(10)
    Q_VARIANT = new(41)
    Q_VARIANT_LIST = new(9)
    Q_VARIANT_MAP = new(8)
    Q_VARIANT_HASH = new(28)
    Q_DATE_TIME = new(16)
    Q_OBJECT_STAR = new(39)
  end
end