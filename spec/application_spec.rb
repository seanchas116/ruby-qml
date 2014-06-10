require 'spec_helper'

describe QML::Application do
  let(:application) { QML::Application.instance }
  describe ".instance" do
    it 'returns the QML::Application instance' do
      expect(application).to be_a(QML::Application)
    end
  end
end
