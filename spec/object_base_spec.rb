require 'spec_helper'

describe QML::ObjectBase do

  describe '#qml_eval' do

    let(:engine) { QML::Engine.new }
    let(:component) do
      c = QML::Component.new(engine)
      c.load_str <<-EOS
        import QtQuick 2.0
        QtObject {
          property string foo: 'foo'
          property string bar: 'bar'
        }
      EOS
    end
    let(:object) { component.create }

    it 'evaluates a JS expression in its QML scope' do
      result = object.qml_eval('foo + bar')
      expect(result).to eq 'foobar'
    end
  end
end
