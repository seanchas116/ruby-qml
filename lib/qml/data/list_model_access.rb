module QML
  class ListModelAccess
    include Access

    signal :begin_insert, [:first, :last]
    signal :end_insert
    signal :begin_move, [:first, :last, :dest]
    signal :end_move
    signal :begin_remove, [:first, :last]
    signal :end_remove
    signal :update, [:first, :last]
    signal :begin_reset
    signal :end_reset

    attr_accessor :model

    def columns
      @model.columns.to_qml
    end

    def data(index, column)
      @model[index.to_i][column.to_sym].to_qml
    end

    def count
      @model.count
    end

    register_to_qml under: 'RubyQml', version: QML::VERSION

    # @return [QML::JSWrapper]
    def self.create(model)
      @component ||= QML::Component.new(data: <<-QML)
        import RubyQml 1.0
        ListModelAccess {}
      QML

      @component.create.tap do |access|
        access.unwrap.model = model
      end
    end
  end
end
