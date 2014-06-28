require 'spec_helper'

describe QML::Reactive::Object do

  class Button
    include QML::Reactive::Object
    signal :pressed, [:pos]
    variadic_signal :message
    property :name, 'button'
    property :id, 0
    property :name_double do
      name + name
    end
    on :pressed do |pos|
      self.name = pos
    end
    on_changed :id do
      self.name = "ID: #{id}"
    end
    on_changed :id do
    end
    alias_property :title, :name
    alias_signal :clicked, :pressed
  end

  class ToggleButton < Button
    signal :pressed, [:x, :y]
    signal :toggled, [:on]
    property(:name) { 'toggle button' }
    property(:info) { 'some info' }
  end

  let(:button) { Button.new }
  let(:toggle_button) { ToggleButton.new }

  describe '.property' do

    it 'defines getter' do
      expect(button.name).to eq('button')
    end

    it 'defines setter' do
      button.name = 'hoge'
      expect(button.name).to eq('hoge')
    end

    it 'can set property binding' do
      button.name = 'poyo'
      expect(button.name_double).to eq 'poyopoyo'
    end

    it 'defines notify signal' do
      spy = QML::Reactive::SignalSpy.new(button.name_changed)
      button.name = 'hoge'
      expect(spy.args.last).to eq(['hoge'])
    end

    it 'returns the name' do
      result = nil
      Class.new do
        include QML::Reactive::Object
        result = property :foo
      end
      expect(result).to eq :foo
    end

    context 'in subclass' do

      it 'defines property in subclass' do
        expect(toggle_button.info).to eq('some info')
      end

      it 'does not define property in superclass' do
        expect(button.properties).not_to include(:info)
      end

      it 'overrides superclass property' do
        expect(toggle_button.name).to eq('toggle button')
      end
    end
  end

  describe '.alias_property' do
    it 'returns the name' do
      result = nil
      Class.new do
        include QML::Reactive::Object
        property :foo
        result = alias_property :bar, :foo
      end
      expect(result).to eq :bar
    end
    it 'aliases a property' do
      expect(button.property(:title)).to eq button.property(:name)
      expect(button.signal(:title_changed)).to eq button.signal(:name_changed)
    end
  end

  describe '.signal' do

    it 'defines a signal' do
      spy = button.pressed.spy
      button.pressed.emit([10, 22])
      expect(spy.args.last).to eq([[10, 22]])
    end

    it 'returns the name' do
      result = nil
      Class.new do
        include QML::Reactive::Object
        result = signal :sig, []
      end
      expect(result).to eq(:sig)
    end

    context 'when the args parameter is not passed' do
      it 'defines a variadic signal' do
        spy = button.message.spy
        button.message.emit(*%w{emitting some message})
        expect(spy.args.last).to eq(%w{emitting some message})
      end
    end

    context 'in subclass' do

      it 'defines signal in subclass' do
        spy = toggle_button.toggled.spy
        toggle_button.toggled.emit(true)
        expect(spy.args.last).to eq([true])
      end

      it 'does not define signal in superclass' do
        expect(button.signals).not_to include(:toggled)
      end

      it 'overrides superclass signal' do
        signal = toggle_button.pressed
        expect(signal.arity).to eq(2)
      end

      it 'defines overridden signal in subclass' do
        spy = toggle_button.pressed.spy
        toggle_button.pressed.emit(10, 20)
        expect(spy.args.last).to eq([10, 20])
      end
    end
  end

  describe '.alias_signal' do

    it 'returns the name' do
      result = nil
      Class.new do
        include QML::Reactive::Object
        signal :foo, []
        result = alias_signal :bar, :foo
      end
      expect(result).to eq :bar
    end

    it 'aliases a signal' do
      expect(button.clicked).to eq button.pressed
    end
  end

  describe '.on' do
    it 'adds signal handler' do
      button.pressed.emit "pressed at top"
      expect(button.name).to eq "pressed at top"
    end
  end

  describe '.on_changed' do
    it 'add property changed signal handler' do
      button.id = 2
      expect(button.name).to eq "ID: 2"
    end
  end

  describe '.instance_signals' do
    context 'when include_super is false' do
      it 'returns all signal definitions of the class' do
        signals = ToggleButton.instance_signals(false)
        expect(signals).to match_array %i{pressed toggled name_changed info_changed}
      end
    end

    context 'when include_super is not specified' do
      it 'returns all signal definitions of the class and its superclasses' do
        signals = ToggleButton.instance_signals
        expect(signals).to match_array %i{pressed message toggled name_changed id_changed info_changed name_double_changed clicked title_changed}
      end
    end
  end

  describe '.instance_properties' do
    context 'when include_super is false' do
      it 'returns all property definitions of the class' do
        properties = ToggleButton.instance_properties(false)
        expect(properties).to match_array %i{name info}
      end
    end

    context 'when include_super is not specified' do
      it 'returns all property definitions of the class and its superclasses' do
        properties = ToggleButton.instance_properties
        expect(properties).to match_array %i{name id info name_double title}
      end
    end
  end

  describe '.instance_signal' do
    it 'returns the UnboundSignal for name' do
      s = ToggleButton.instance_signal(:pressed)
      expect(s).to be_a QML::Reactive::UnboundSignal
      expect(s.name).to eq :pressed
      expect(s.arity).to eq 2
    end
    context 'when signal does not exist' do
      it 'fails with NameError' do
        expect { ToggleButton.instance_signal(:non_existent) }.to raise_error(NameError)
      end
    end
  end

  describe '.instance_property' do
    it 'returns the UnboundProperty for name' do
      p = ToggleButton.instance_property(:name)
      expect(p).to be_a QML::Reactive::UnboundProperty
      expect(p.name).to eq :name
      expect(p.initial_binding.call).to eq 'toggle button'
    end
    context 'when property does not exist' do
      it 'fails with NameError' do
        expect { ToggleButton.instance_property(:non_existent) }.to raise_error(NameError)
      end
    end
  end

  describe '#signals' do
    it 'returns signal names for object' do
      expect(toggle_button.signals).to match_array %i{pressed message toggled id_changed name_changed info_changed name_double_changed clicked title_changed}
    end
  end

  describe '#properties' do
    it 'return property names for object' do
      expect(toggle_button.properties).to match_array %i{name id info name_double title}
    end
  end

  describe '#signal' do
    it 'returns Signal for name' do
      expect(toggle_button.signal(:pressed)).to eq toggle_button.pressed
    end
    context 'when signal does not exist' do
      it 'fails with NameError' do
        expect { toggle_button.signal(:non_existent) }.to raise_error(NameError)
      end
    end
  end

  describe '#property' do
    it 'returns Property for name' do
      expect(toggle_button.property(:name).value).to eq toggle_button.name
    end
    context 'when property does not exist' do
      it 'fails with NameError' do
        expect { toggle_button.property(:non_existent) }.to raise_error(NameError)
      end
    end
  end
end
