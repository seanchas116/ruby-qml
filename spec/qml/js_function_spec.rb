require 'spec_helper'

describe QML::JSFunction do

  let(:function_script) do
    <<-JS
      (function(a, b) {
        return a + b;
      });
    JS
  end
  let(:function) { QML.engine.evaluate(function_script) }

  describe '#call' do
    it 'calls function' do
      expect(function.call(1,2)).to eq 3
    end
  end
end
