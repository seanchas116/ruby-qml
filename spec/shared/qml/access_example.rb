
class AccessExample
  include QML::Access

  property :text
  signal :some_signal, [:arg]

  def some_method(a, b)
    a + b
  end

  register_to_qml under: 'AccessExampleNS', version: '1.2', name: 'AccessExample'
end

module AccessExampleModule
  VERSION = '0.1.0'
  class AccessExample
    include QML::Access
    register_to_qml
  end
end
