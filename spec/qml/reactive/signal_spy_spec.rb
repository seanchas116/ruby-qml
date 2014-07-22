require 'spec_helper'

describe QML::Reactive::SignalSpy do

  it 'records signal emittion' do
    signal = QML::Reactive::Signal.new([:arg1, :arg2])
    spy = QML::Reactive::SignalSpy.new(signal)
    signal.emit(10, 20)
    signal.emit('foo', 'bar')
    expect(spy.args).to eq([[10, 20], ['foo', 'bar']])
  end

end

describe QML::Reactive::Signal do

  describe '#spy' do
    it 'returns a SignalSpy for the signal' do
      signal = QML::Reactive::Signal.new([:arg1, :arg2])
      spy = signal.spy
      signal.emit(10, 20)
      signal.emit('foo', 'bar')
      expect(spy.args).to eq([[10, 20], ['foo', 'bar']])
    end
  end
end