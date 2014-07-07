require 'spec_helper'

describe QML::Dispatchable do
  class DispatchableFoo
    include QML::Dispatchable
    attr_accessor :value
  end

  describe '#later' do
    it 'queues a method call as a task to the dispatcher' do
      foo = DispatchableFoo.new
      foo.later.value = 'hoge'
      expect(foo.value).to be_nil
      QML.application.process_events
      expect(foo.value).to eq 'hoge'
    end
  end
end

describe QML::Access do
  it 'includes Dispatchable' do
    expect(QML::Access.include?(QML::Dispatchable)).to eq true
  end
end

describe QML::Wrapper do
  it 'includes Dispatchable' do
    expect(QML::Wrapper.include?(QML::Dispatchable)).to eq true
  end
end
