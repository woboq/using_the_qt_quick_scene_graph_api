import main 1.0
import QtQuick 2.4

Item {
  property string text
  property real revealProgress: 1
  property QtObject helper: QRCodeHelper { }
  Rectangle {
    id: qrCode2
    property var qrCodeData: helper.getQRCodeData(text)
    // Keep space for an empty dot length at the edges
    width: qrCodeData.width + 2
    height: width

    // This item is using a coordinate system where one QR code dot is 1 unit
    // Apply a scale to fill the parent in which 1 unit is 1 pixel
    scale: parent.width / width
    transformOrigin: Item.TopLeft

    color: "white"
    Repeater {
      model: qrCode2.qrCodeData.dots
      delegate: Rectangle {
        property real dotProgress: {
          function smoothstep(edge0, edge1, x) {
            var t = Math.min(1.0, Math.max(0.0, (x - edge0) / (edge1 - edge0)));
            return t * t * (3.0 - 2.0 * t);
          }
          var start = 1.0 - smoothstep(0.0, 30, modelData.surroundCount)
          var end = 1.0 - smoothstep(0.0, 30, modelData.surroundCount + 5.0)
          return smoothstep(start, end, revealProgress)
        }
        color: "black"
        opacity: 1 - dotProgress
        // + 1 dot offset for the empty edge
        x: modelData.x + 1
        y: modelData.y + 1 + 25 * 0.5 * dotProgress * dotProgress
        width: 1
        height: 1
      }
    }
  }
}