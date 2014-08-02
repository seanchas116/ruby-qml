shared_context 'Reactive test objects' do
  let(:button_class) do
    Class.new do
      include QML::Reactive::Object
      signal :pressed, [:pos]
      variadic_signal :message
      property :name, 'button'
      property :id, 0
      property :name_double do
        name + name
      end
      on :pressed do |pos|
        self.name = pos
      end
      on_changed :id do
        self.name = "ID: #{id}"
      end
      on_changed :id do
      end
      alias_property :title, :name
      alias_signal :clicked, :pressed
    end
  end

  let(:toggle_button_class) do
    Class.new(button_class) do
      signal :pressed, [:x, :y]
      signal :toggled, [:on]
      property(:name) { 'toggle button' }
      property(:info) { 'some info' }
      on_changed :id do
        self.info = 'ID changed'
      end
    end
  end

  let(:button) { button_class.new }
  let(:toggle_button) { toggle_button_class.new }
end
