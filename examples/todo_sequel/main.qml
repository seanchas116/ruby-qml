import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Examples.Todo 0.1

ApplicationWindow {
    visible: true
    title: "Todo with Sequel"

    FontLoader {
        source: "../assets/fonts/fontawesome-webfont.ttf"
    }
    id: window
    property int margin: 10
    width: layout.implicitWidth + 2 * margin
    height: layout.implicitHeight + 2 * margin

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: window.margin
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
        ColumnLayout {
            RowLayout {
                Label { text: "Sort by" }
                ComboBox {
                    id: orderComboBox
                    model: ListModel {
                        ListElement { text: "Title"; column: "title" }
                        ListElement { text: "Description"; column: "description" }
                        ListElement { text: "Due Date"; column: "due_date" }
                    }
                    property string currentColumn: model.get(currentIndex).column
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
                            text: Qt.formatDate(due_date)
                        }
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
        order_by: orderComboBox.currentColumn
    }
}
