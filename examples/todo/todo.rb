$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'pathname'

module Examples
  module Todo
    VERSION = '0.1'

    class TodoModel < QML::Data::ArrayModel
      column :title, :description, :due_date
    end

    class TodoController
      include QML::Access
      register_to_qml

      property :title, ''
      property :description, ''
      property :due_date, ''
      property :model, TodoModel.new

      def add
        item = OpenStruct.new(
          title: title,
          description: description,
          due_date: due_date)
        p item
        model << item
      end
    end
  end
end

QML.application do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
