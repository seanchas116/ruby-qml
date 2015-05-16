
module QML
  module_function

  # @example
  #   def set_app_name
  #     QML.qt.application.name = 'appname'
  #   end
  # @return [JSObject] QML Qt namespace object
  # @see http://doc.qt.io/qt-5/qml-qtqml-qt.html
  def qt
    @qt ||= begin
      component = QML::Component.new data: <<-QML
        import QtQuick 2.0
        QtObject {
          function getQt() {
            return Qt;
          }
        }
      QML
      component.create.getQt
    end
  end
end
