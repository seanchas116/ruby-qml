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
    'Date' => Date.today,
    'Access derived' => Foo.new,
    'QML Point' => QML::Geometry::Point.new(1,2),
    'QML Size' => QML::Geometry::Size.new(1,2),
    'QML Rectangle' => QML::Geometry::Rectangle.new(1,2,3,4)
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

  it 'can convert DateTime' do
    time = DateTime.now
    result = convert(time)
    # QDateTime has msec precision
    expect((result.to_time.to_r * 1000).to_i).to eq (time.to_time.to_r * 1000).to_i
    expect(result.to_time.gmt_offset).to eq(result.to_time.gmt_offset)
  end

  it 'can convert meta object' do
    metaobj = QML::MetaObject.new
    actual = convert(metaobj)
    expect(actual).to eq metaobj
  end

end
