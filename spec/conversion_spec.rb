require 'spec_helper'

def convert(value)
  QML::TestUtil.echo_conversion(value)
end

describe 'Conversion between C++ and Ruby' do

  class Foo
    include QML::Access
  end

  samples = {
    'Integer' => 1234,
    'true' => true,
    'false' => false,
    'nil' => nil,
    'Float' => 3.1416,
    'String' => 'hoge',
    'non-ascii String' => 'ほげ',
    'Array' => [1, "foobar", true],
    'Hash' => {'one' => 1, 'two' => 2, '三' => 3},
    'Access derived' => Foo.new,
    'QML Point' => QML::Point.new(1,2),
    'QML Size' => QML::Size.new(1,2),
    'QML Rectangle' => QML::Rectangle.new(1,2,3,4)
  }

  samples.each do |k, v|
    it "can convert #{k}" do
      actual = convert(v)
      expect(actual).to eq v
    end
  end

  it 'can convert Symbol as String' do
    expected = :hogehoge
    actual = convert(expected)
    expect(actual).to eq expected.to_s
  end

  it 'can convert Time' do
    time = Time.now
    result = convert(time)
    # QDateTime has msec precision
    expect((result.to_r * 1000).to_i).to eq (time.to_r * 1000).to_i
    expect(result.gmt_offset).to eq(result.gmt_offset)
  end

  it 'can convert meta object' do
    metaobj = QML::MetaObject.new
    actual = convert(metaobj)
    expect(actual).to eq metaobj
  end

end
