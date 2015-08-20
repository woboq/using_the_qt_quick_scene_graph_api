#include "qrcode.h"

#include <QSGGeometryNode>
#include <QSGSimpleMaterialShader>
#include <QSGSimpleRectNode>
#include <qrencode.h>

static float smoothstep(float edge0, float edge1, float x) {
  float t =
    std::min(1.0f,
    std::max(0.0f,
      (x - edge0) / (edge1 - edge0)));
  return t * t * (3.0f - 2.0f * t);
}

class QRDotNode : public QSGNode {
public:
  QRDotNode(const QRectF &rect, int surroundCount)
  : m_translateNode(new QSGTransformNode)
  , m_opacityNode(new QSGOpacityNode)
  , m_surroundCount(surroundCount)
  {
    auto geometryNode = new QSGSimpleRectNode(rect, Qt::red);
    m_opacityNode->appendChildNode(geometryNode);
    m_translateNode->appendChildNode(m_opacityNode);
    // This takes ownership of the child node tree
    appendChildNode(m_translateNode);
  }

  void setRevealProgress(float revealProgress)
  {
    float start = 1.0 - smoothstep(0.0, 30, m_surroundCount);
    float end = 1.0 - smoothstep(0.0, 30, m_surroundCount + 5.0);
    float dotProgress = smoothstep(start, end, revealProgress);
    QMatrix4x4 translateMatrix;
    translateMatrix.translate(
      0, 25 * 0.5 * dotProgress * dotProgress
      );
    m_translateNode->setMatrix(translateMatrix);
    m_opacityNode->setOpacity(1 - dotProgress);
  }

private:
  QSGTransformNode *m_translateNode;
  QSGOpacityNode *m_opacityNode;
  int m_surroundCount;
};

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
  m_textDirty = true;
  update();
}

void QRCodeItem::setRevealProgress(float revealProgress)
{
  if (m_revealProgress == revealProgress)
    return;
  m_revealProgress = revealProgress;
  emit revealProgressChanged();
  update();
}

QSGNode *QRCodeItem::updatePaintNode(QSGNode *oldNode,
  UpdatePaintNodeData *)
{
  auto rootScaleNode = static_cast<QSGTransformNode *>(oldNode);
  if (!rootScaleNode) {
    // Either this is the first run or
    // the scene graph destroyed our paint node
    rootScaleNode = new QSGTransformNode;
    rootScaleNode->appendChildNode(new QSGSimpleRectNode(QRectF(), Qt::white));
    rootScaleNode->appendChildNode(new QSGNode);
  }
  auto bgNode = static_cast<QSGSimpleRectNode *>(
    rootScaleNode->firstChild());
  auto dotContainer = rootScaleNode->lastChild();

  if (m_textDirty) {
    m_textDirty = false;

    // Delete all QRDotNodes, children of dotContainer.
    while (auto dot = dotContainer->firstChild())
      delete dot;

    const Code code = getQRCodeData(m_text).value<Code>();
    int contentWidth = code.width + 2;
    bgNode->setRect(0, 0, contentWidth, contentWidth);

    for (int i = 0; i < code.dots.size(); ++i) {
      CodeDot dot = code.dots.at(i).value<CodeDot>();
      // + 1 dot offset for the empty edge
      dotContainer->appendChildNode(
        new QRDotNode(
          QRectF(dot.x + 1, dot.y + 1, 1, 1),
          dot.surroundCount)
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

  for (auto dot = static_cast<QRDotNode *>(dotContainer->firstChild())
    ; dot; dot = static_cast<QRDotNode *>(dot->nextSibling()))
    dot->setRevealProgress(m_revealProgress);

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

