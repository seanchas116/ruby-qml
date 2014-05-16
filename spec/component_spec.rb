require 'spec_helper'

describe QML::Component do

  let(:engine) { QML::Engine.new }
  let(:component) { QML::Component.new(engine) }

  it 'instantiates a object from string' do
    component.load_str <<-EOS
      import QtQuick 2.0
      QtObject {
        property string name: 'foo'
      }
    EOS
    expect(component.create.name).to eq 'foo'
  end

  it 'instantiates a object from file' do
    component.load_file QML::ROOT_PATH + 'spec/assets/testobj.qml'
    expect(component.create.name).to eq 'foo'
  end

end
