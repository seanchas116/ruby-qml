import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Examples.Todo 0.1

ApplicationWindow {
    visible: true
    title: "Todo Array Model"

    FontLoader {
        source: "../assets/fonts/fontawesome-webfont.ttf"
    }
    width: layout.implicitWidth
    height: layout.implicitHeight

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 10
        ColumnLayout {
            TextField {
                placeholderText: "Title"
                id: titleField
            }
            TextField {
                placeholderText: "Description"
                id: descriptionField
            }
            Calendar {
                id: calendar
            }
            Button {
                text: "Add"
                onClicked: todo.add()
            }
        }
        ListView {
            model: todo.model
            spacing: 10
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 300
            delegate: ColumnLayout {
                Text {
                    font.bold: true
                    text: title
                }
                Text {
                    text: description
                }
                RowLayout {
                    Text {
                        font.family: "FontAwesome"
                        text: "\uf073"
                    }
                    Text {
                        text: Qt.formatDateTime(due_date)
                    }
                }
            }
        }
    }
    TodoController {
        id: todo
        title: titleField.text
        description: descriptionField.text
        due_date: calendar.selectedDate
    }
}
