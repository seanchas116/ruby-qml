
class Numeric
  def to_qml
    to_f
  end
end

class String
  def to_qml
    self
  end
end

class Symbol
  def to_qml
    to_s
  end
end

class TrueClass
  def to_qml
    self
  end
end

class FalseClass
  def to_qml
    self
  end
end

class NilClass
  def to_qml
    self
  end
end

class Array
  def to_qml
    QML.engine.new_array(self.size).tap do |jsarray|
      self.each_with_index do |x, i|
        jsarray[i] = x
      end
    end
  end
end

class Hash
  def to_qml
    QML.engine.new_object.tap do |jsobj|
      self.each do |key, value|
        jsobj[key] = value
      end
    end
  end
end

class Time
  def to_qml
    QML::JSUtil.classes['Date'].new(year, month, day, hour, min, sec, nsec / 1000000).tap do |date|
      date.setTime((to_r * 1000).floor)
    end
  end
end
