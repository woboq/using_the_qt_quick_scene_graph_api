import QtQuick.Controls 1.0
import QtQuick 2.0
import main 1.0

Item {
  height: 600
  width: 600

  Grid {
    id: grid
    columns: 1
    rows: columns
    anchors.fill: parent

    Repeater {
      model: grid.columns * grid.rows
      delegate: QRCodeItem {
        width: grid.width / grid.columns
        height: grid.height / grid.rows
        text: "http://woboq.com/software-services.html"
        // revealProgress: slider.value
      }
    }        
  }

  Slider {
    id: slider
    property QtObject animation: SequentialAnimation {
      loops: Animation.Infinite
      NumberAnimation { to: 0; target: slider; property: "value"; duration: 2000 }
      NumberAnimation { to: 1; target: slider; property: "value"; duration: 2000 }
    }
    anchors { left: parent.left; top: parent.top; right: parent.right }
    value: 1
  }

  focus: true
  Keys.onPressed: {
    if (event.key == Qt.Key_Left)
      grid.columns = Math.max(1, grid.columns * 0.5)
    else if (event.key == Qt.Key_Right)
      grid.columns *= 2
    else if (event.key == Qt.Key_Space)
      slider.animation.running = !slider.animation.running

    print("Showing " + grid.columns * grid.columns + " items.")
  }
}