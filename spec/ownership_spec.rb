require 'spec_helper'

describe QML::QtObjectBase do

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
end
