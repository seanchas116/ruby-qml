$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'qml'
require 'pathname'

module Examples
  module Todo
    VERSION = '0.1'

    class TodoController
      include QML::Access

      property(:title) { '' }
      property(:description) { '' }
      property(:due_date) { '' }
      property(:model) { QML::ArrayModel.new(:title, :description, :due_date) }

      def add
        item = {
          title: title,
          description: description,
          due_date: due_date
        }
        p item
        model << item
      end

      register_to_qml
    end
  end
end

QML.run do |app|
  app.load_path Pathname(__FILE__) + '../main.qml'
end
