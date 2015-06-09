require 'spec_helper'

describe "QML signal connection" do

  let(:component) do
    QML::Component.new(data: <<-QML)
      import QtQuick 2.0
      QtObject {
        signal someSignal(var arg)
      }
    QML
  end

  let(:obj) { component.create }

  it "connects QML signals to ruby proc" do
    received_arg = nil

    obj[:someSignal].connect do |arg|
      received_arg = arg
    end

    obj.someSignal(10)

    expect(received_arg).to eq(10)
  end
end
