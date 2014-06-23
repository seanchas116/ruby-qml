import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Examples.ImageProvider 0.1

ApplicationWindow {
  visible: true
  width: 200
  height: 200
  GridLayout {
    anchors.fill: parent
    anchors.margins: 10
    columns: 1
    Image {
      id: image
      source: 'image://example/image'
      //source: 'https://pbs.twimg.com/profile_images/432861545521872897/BsrVF3et_bigger.png'
      fillMode: Image.Pad
      Layout.fillHeight: true
      function reload() {
        source = ''
        source = 'image://example/image'
      }
    }
    GridLayout {
      columns: 2
      Label {
        text: 'Image URL'
      }
      TextField {
        id: textField
        Layout.fillWidth: true
        onEditingFinished: {
          image.reload()
        }
      }
    }
  }

  Controller {
    id: provider
    url: textField.text
  }
}
