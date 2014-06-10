import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.FizzBuzz 0.1

ApplicationWindow {
    visible: true
    width: 200
    height: 100
    title: qsTr("FizzBuzz")

    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        columnSpacing: 20
        Text {
            text: "Input"
            Layout.alignment: Qt.AlignRight
        }
        TextField {
            id: textField
        }
        Text {
            text: "Result"
            Layout.alignment: Qt.AlignRight
        }
        Text {
            y: 100
            id: text
            text: fizzbuzz.result
        }
        Item {
            Layout.fillHeight: true
        }
    }
    FizzBuzz {
        id: fizzbuzz
        input: textField.text
    }
}
