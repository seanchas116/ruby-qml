shared_context 'ListView for model available' do
  let(:component) do
    QML::Component.new data: <<-EOS
      import QtQuick 2.0
      ListView {
        model: ListModel {}
        delegate: Item {
          property var itemTitle: title
          property var itemNumber: number
        }
      }
    EOS
  end
  let(:list_view) { component.create }
end

shared_examples 'ListView data source' do
  it 'updates ListView correctly' do
    list_view.model = model

    count = list_view['count'].to_i

    expect(count).to eq expected_array.size

    count.times do |i|
      list_view.currentIndex = i
      current = list_view.currentItem
      expect(current.itemTitle).to eq(expected_array[i][:title])
      expect(current.itemNumber).to eq(expected_array[i][:number])
    end
  end
end
