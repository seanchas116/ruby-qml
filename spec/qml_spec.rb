require 'spec_helper'

describe QML do
  it 'should have a version number' do
    expect(QML::VERSION).not_to be_nil
  end
end
