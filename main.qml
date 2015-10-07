// The MIT License (MIT)
//
// Copyright (c) 2015 Woboq GmbH <contact at woboq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
        revealProgress: slider.value
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