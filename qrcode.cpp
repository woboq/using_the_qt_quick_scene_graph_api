#include "qrcode.h"

#include <QSGGeometryNode>
#include <QSGSimpleMaterialShader>
#include <QSGSimpleRectNode>
#include <qrencode.h>


QRCodeItem::QRCodeItem()
{
  setFlag(ItemHasContents);
}

void QRCodeItem::setText(const QString &text)
{
  if (m_text == text)
    return;
  m_text = text;
  emit textChanged();
  update();
}

QSGNode *QRCodeItem::updatePaintNode(QSGNode *oldNode,
  UpdatePaintNodeData *)
{
  auto rootScaleNode = static_cast<QSGTransformNode *>(oldNode);
  if (!rootScaleNode && !m_text.isEmpty()) {
    rootScaleNode = new QSGTransformNode;

    const Code code = getQRCodeData(m_text).value<Code>();
    int contentWidth = code.width + 2;
    rootScaleNode->appendChildNode(
      new QSGSimpleRectNode(
        QRectF(0, 0, contentWidth, contentWidth),
        Qt::white)
      );

    for (int i = 0; i < code.dots.size(); ++i) {
      CodeDot dot = code.dots.at(i).value<CodeDot>();
      // + 1 dot offset for the empty edge
      rootScaleNode->appendChildNode(
        new QSGSimpleRectNode(
          QRectF(dot.x + 1, dot.y + 1, 1, 1),
          Qt::red)
        );
    }

    // We put geometries in a coordinate system where all
    // points are 1 unit wide. Scale the whole QR code so
    // that it fills our item's width. We should normally
    // also consider its height.
    QMatrix4x4 scaleMatrix;
    scaleMatrix.scale(width() / contentWidth);
    rootScaleNode->setMatrix(scaleMatrix);
  }

  return rootScaleNode;
}

QVariant QRCodeItem::getQRCodeData(const QString &text)
{
  QRcode *qrencodeCode = QRcode_encodeString(
    text.toUtf8().constData(), 1, QR_ECLEVEL_L, QR_MODE_8, true);
  Code code;
  code.width = qrencodeCode ? qrencodeCode->width : 0;

  for (int y = 0; y < code.width; ++y) {
    for (int x = 0; x < code.width; ++x) {
      auto src = qrencodeCode->data + y * code.width + x;
      // First bit: 1=black/0=white
      if (*src & 0x1) {
        // Count the number of dots 2 columns/rows around
        int surroundCount = 0;
        for (int i = x - 2; i <= x + 2; ++i) {
          if (i < 0 || i >= code.width)
            continue;
          for (int j = y - 2; j <= y + 2; ++j) {
            if (j < 0 || j >= code.width)
              continue;
            auto src = qrencodeCode->data + j * code.width + i;
            if (*src & 0x1)
              ++surroundCount;
          }
        }

        code.dots.append(QVariant::fromValue(
          CodeDot{x, y, surroundCount}
          ));
      }
    }
  }
  QRcode_free(qrencodeCode);

  return QVariant::fromValue(code);
}

