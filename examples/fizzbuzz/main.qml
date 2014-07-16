import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.FizzBuzz 0.1

ApplicationWindow {
    visible: true
    width: 200
    height: 200
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
            text: fizzBuzz.result
        }
        Button {
            text: 'Quit'
            onClicked: fizzBuzz.quit()
        }
        Text {
            id: lastFizzBuzz
        }
    }
    FizzBuzz {
        id: fizzBuzz
        input: textField.text
        onInputWasFizzBuzz: lastFizzBuzz.text = "Last FizzBuzz: " + textField.text
    }
}
