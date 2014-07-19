import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.Twitter 0.1

ApplicationWindow {
    visible: true
    width: 300
    height: 500
    title: "Twitter Search Test - " + controller.word

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        TextField {
            id: wordField
            placeholderText: 'Search...'
            onEditingFinished: controller.fetch_tweets()
        }
        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ListView {
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
        }
    }
    TwitterController {
        id: controller
        word: wordField.text
    }
}
