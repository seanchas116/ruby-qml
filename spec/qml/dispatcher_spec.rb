require 'spec_helper'

describe QML::Dispatcher do

  let(:dispatcher) { QML::Dispatcher.instance }

  describe '#empty?' do
    context 'when it has no task' do
      it 'returns true' do
        dispatcher.run_tasks until dispatcher.empty?
        expect(dispatcher.empty?).to eq true
      end
    end
    context 'when it has tasks' do
      it 'returns false' do
        dispatcher.add_task {}
        expect(dispatcher.empty?).to eq false
      end
    end
  end

  describe '#run_tasks' do
    it 'runs queued tasks' do
      finished = []
      2.times.each do |i|
        dispatcher.add_task do
          finished[i] = true
        end
      end
      dispatcher.run_tasks until dispatcher.empty?
      expect(finished).to eq [true, true]
    end
  end
end

describe QML do
  describe '.later' do
    it 'adds a task to the dispatcher and do it later in event loop' do
      finished = false
      QML.later do
        finished = true
      end
      expect(finished).to eq false
      QML.application.process_events
      expect(finished).to eq true
    end
  end
end
