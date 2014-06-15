require 'spec_helper'

describe QML::QtObjectPointer do
  let(:create_test_object) { -> { QML::Plugins.testobject.create_test_object } }
  let(:pointer) { create_test_object.call.object_pointer }

  describe '#destroy!' do
    context 'when owned' do
      before do
        pointer.owned = true
      end
      it 'destroyes the object' do
        pointer.destroy!
        expect(pointer.null?).to eq true
      end
    end
    context 'when not owned' do
      before do
        pointer.owned = false
      end
      it 'fails with QtObjectError' do
        expect { pointer.destroy! }.to raise_error QML::QtObjectError
      end
    end
  end

  describe '#owned?' do
    [true, false].each do |cond|
      context "when owned = #{cond}" do
        before { pointer.owned = cond }
        it "returns #{cond}" do
          expect(pointer.owned?).to eq cond
        end
      end
    end
  end

  context 'when garbage collected' do
    context 'when owned' do
      it 'destroyes the object' do
        pointer = lambda do
          owned_pointer = create_test_object.call.object_pointer
          owned_pointer.owned = true
          pointer = owned_pointer.dup
          pointer.owned = false
          pointer
        end.call
        GC.start
        expect(pointer.null?).to eq true
      end
    end
    context 'when not owned' do
      it 'does not destroy the object' do
        pointer = lambda do
          owned_pointer = create_test_object.call.object_pointer
          owned_pointer.owned = false
          pointer = owned_pointer.dup
          pointer.owned = false
          pointer
        end.call
        GC.start
        expect(pointer.null?).to eq false
      end
    end
  end
end
