shared_context 'ListView for model available' do
  let(:context) { QML::Context.new }
  let(:component) do
    QML::Component.new context: context, data: <<-EOS
      import QtQuick 2.0
      ListView {
        model: the_model
        delegate: Item {
          property var itemTitle: title
          property var itemNumber: number
        }
      }
    EOS
  end
  let(:list_view) { component.create }

  before do
    context[:the_model] = model
  end
end

shared_examples 'ListView data source' do
  it 'updates ListView correctly' do
    count = list_view.count
    expect(count).to eq expected_array.size
    list_view.count.times do |i|
      list_view.current_index = i
      current = list_view.current_item
      expect(current.item_title).to eq(expected_array[i][:title])
      expect(current.item_number).to eq(expected_array[i][:number])
    end
  end
end
