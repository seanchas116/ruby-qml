
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
