$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'sequel'

module Examples
  module Todo
    VERSION = '0.1'

    DB = Sequel.sqlite

    DB.create_table :todos do
      primary_key :id
      String :title
      String :description
      Time :due_date
    end

    class SequelModel < QML::Data::QueryModel
      attr_accessor :dataset

      def initialize(dataset)
        @dataset = dataset
        super(*dataset.columns)
      end

      def query_count
        @dataset.count
      end

      def query(offset, count)
        @dataset.offset(offset).limit(count).all
      end
    end

    class TodoController
      include QML::Access
      register_to_qml

      def initialize
        super
        @todo_dataset = DB[:todos]
        self.model = SequelModel.new(@todo_dataset)
      end

      property(:title) { '' }
      property(:description) { '' }
      property(:due_date) { '' }
      property(:order_by) { '' }
      property :model

      def add
        @todo_dataset.insert(title: title, description: description, due_date: due_date.to_time)
        model.update
      end

      on_changed :order_by do
        model.dataset = @todo_dataset.order(order_by.to_sym)
        model.update
      end
    end
  end
end

QML.run do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
