import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.Twitter 0.1

ApplicationWindow {
    visible: true
    width: 200
    height: 100
    title: "Twitter Test"

    ListView {
        anchors.fill: parent
        model: controller.model
        delegate: RowLayout {
            Image {
                width: 100
                height: 100
                source: user_icon
            }
            ColumnLayout {
                Text {
                    font.bold: true
                    text: user_name
                }
                Text {
                    text: tweet_text
                }
            }
        }
    }

    TwitterController {
        id: controller
    }
}
