require 'spec_helper'

describe QML::QtObjectBase do

  let(:testobject_plugin) { QML::Plugins.testobject }
  let(:ownership_test) { testobject_plugin.create_ownership_test }

  context 'when it is returned from a method' do
    it 'gets ownership of the object' do
      deleted = false
      ownership_test.created_object_deleted.each do
        deleted = true
      end
      ownership_test.create_object
      GC.start
      QML.application.force_deferred_deletes
      expect(deleted).to eq true
    end
  end
  context 'when it is obtained from a property' do
    it 'does not get ownership of the object' do
      deleted = false
      ownership_test.property_object_deleted.each do
        deleted = true
      end
      ownership_test.property_object
      GC.start
      QML.application.force_deferred_deletes
      expect(deleted).to eq false
    end
  end
  context 'when it has a parent' do
    it 'does not get ownership of the object' do
      deleted = false
      ownership_test.sub_object_deleted.each do
        deleted = true
      end
      ownership_test.sub_object
      GC.start
      QML.application.force_deferred_deletes
      expect(deleted).to eq false
    end
  end
end
