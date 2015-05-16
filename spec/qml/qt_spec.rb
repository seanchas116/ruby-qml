require 'spec_helper'

describe QML do
  describe '.qt' do
    it 'returns the Qt object' do
      expect(QML.qt.md5('hoge')).to eq('ea703e7aa1efda0064eaa507d9e8ab7e')
    end
  end
end
