require 'spec_helper'

describe QML::Reactive do
  let(:button_class) do
    Class.new do
      include QML::Reactive
      signal :pressed
      property(:is_pressed) { false }
      property :id
      property(:is_id_changed) { false }

      on :pressed do |pos|
        self.is_pressed = true
      end
      on_changed :id do
        self.is_id_changed = true
      end
    end
  end
  let(:button) { button_class.new }

  describe '.on' do
    it 'registers a signal listener' do
      expect(button.is_pressed).to eq(false)
      button.pressed.emit
      expect(button.is_pressed).to eq(true)
    end
  end

  describe '.on_changed' do
    it 'registers a property change listener' do
      expect(button.is_id_changed).to eq(false)
      button.id = "foo"
      expect(button.is_id_changed).to eq(true)
    end
  end
end
