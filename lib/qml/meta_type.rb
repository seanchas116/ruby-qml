module QML
  class MetaType

    def self.from_name(name)
      new(CLib.rbqml_metatype_from_name(name))
    end

    attr_reader :id

    def initialize(id)
      @id = id
    end

    def name
      @name ||= CLib.rbqml_metatype_name(@id).to_sym
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

    def valid?
      self != UNKNOWN
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
      when Q_OBJECT_STAR
        QtObjectBase
      when VOID_STAR
        FFI::Pointer
      when CONST_Q_META_OBJECT_STAR
        MetaObject
      else
        nil
      end
    end

    UNKNOWN = new(0)
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
    VOID_STAR = new(31)
    CONST_Q_META_OBJECT_STAR = from_name('const QMetaObject *')
  end
end