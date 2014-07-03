import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.FizzBuzz 0.1

ApplicationWindow {
    visible: true
    width: 200
    height: 100
    title: "FizzBuzz"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        TextField {
            placeholderText: "Input"
            text: "0"
            id: textField
        }
        Text {
            y: 100
            id: text
            text: fizzbuzz.result
        }
        Button {
            text: 'Quit'
            onClicked: fizzbuzz.quit()
        }
    }
    FizzBuzz {
        id: fizzbuzz
        input: textField.text
    }
}
