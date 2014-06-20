require 'spec_helper'

describe QML::QtObjectBase do

  describe '#qml_eval' do

    let(:engine) { QML::Engine.new }
    let(:component) do
      QML::Component.new engine, data: <<-EOS
        import QtQuick 2.0
        QtObject {
          property string foo: 'foo'
          property string bar: 'bar'
        }
      EOS
    end
    let(:object) { component.create }
    let(:expression) { 'foo + bar' }
    let(:result) { object.qml_eval(expression) }

    it 'evaluates a JS expression in its QML scope' do
      expect(result).to eq 'foobar'
    end

    context 'when expression is wrong' do
      let(:expression) { 'hoge + piyo' }
      it 'raises QML::QMLError' do
        expect { result }.to raise_error(QML::QMLError)
      end
    end

    context 'when object does not belong to context' do
      let(:object) { QML::Plugins.test_util.createTestObject }
      it 'raises QML::QMLError' do
        expect { result }.to raise_error(QML::QMLError)
      end
    end
  end

  describe 'ownership' do
    let(:ownership_test) { QML::Plugins.test_util.create_ownership_test }

    context 'when it is returned from a method' do
      it 'gets ownership of the object' do
        checker = -> { QML::TestUtil::ObjectLifeChecker.new(ownership_test.create_object) }.call
        GC.start
        QML.application.force_deferred_deletes
        expect(checker.alive?).to eq false
      end
    end

    context 'when it is obtained from a property' do
      it 'does not get ownership of the object' do
        checker = -> { QML::TestUtil::ObjectLifeChecker.new(ownership_test.property_object) }.call
        GC.start
        QML.application.force_deferred_deletes
        expect(checker.alive?).to eq true
      end
    end

    context 'when it has a parent' do
      it 'does not get ownership of the object' do
        checker = -> { QML::TestUtil::ObjectLifeChecker.new(ownership_test.sub_object) }.call
        GC.start
        QML.application.force_deferred_deletes
        expect(checker.alive?).to eq true
      end
    end

    describe '#owned_by_ruby?' do
      [true, false].each do |cond|
        context "when owned_by_ruby = #{cond}" do
          it "returns #{cond}" do
            obj = ownership_test.create_object
            obj.owned_by_ruby = cond
            expect(obj.owned_by_ruby?).to eq cond
          end
        end
      end
    end

    context 'when garbage collected' do

      context 'when owned' do
        it 'destroyes the object' do
          checker = -> {
            obj = ownership_test.create_object
            obj.owned_by_ruby = true
            QML::TestUtil::ObjectLifeChecker.new(obj)
          }.call
          GC.start
          QML.application.force_deferred_deletes
          expect(checker.alive?).to eq false
        end
      end

      context 'when not owned' do
        it 'does not destroy the object' do
          checker = -> {
            obj = ownership_test.create_object
            obj.owned_by_ruby = false
            QML::TestUtil::ObjectLifeChecker.new(obj)
          }.call
          GC.start
          QML.application.force_deferred_deletes
          expect(checker.alive?).to eq true
        end
      end
    end
  end
end
