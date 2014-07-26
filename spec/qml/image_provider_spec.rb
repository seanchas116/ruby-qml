require 'spec_helper'

describe QML::ImageProvider do

  describe '#request' do
    it 'is not implemented by default' do
      expect { QML::ImageProvider.new.request(nil) }.to raise_error(NotImplementedError)
    end
  end

  context 'with test image' do
    let(:klass) do
      Class.new QML::ImageProvider do
        attr_reader :image

        def initialize
          @image = (Pathname(__FILE__) + '../../assets/test.png').binread
          super
        end

        def request(req)
          req.finish(image)
        end
      end
    end
    let(:image_provider) { klass.new }

    it 'provides image to QML' do
      test = QML::Plugins.test_util.create_image_provider_test
      expect(test.request_and_compare(image_provider.qt_image_provider, 'test', image_provider.image)).to eq true
    end
  end
end
