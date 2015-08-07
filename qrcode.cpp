#include "qrcode.h"

#include <QSGGeometryNode>
#include <QSGSimpleMaterialShader>
#include <QSGSimpleRectNode>
#include <qrencode.h>

namespace {

// This is the C++ structure used to write the memory layout
// matching our vertex attributes.
struct QRCodeVertex {
  float x, y;
  float surroundCount;
};

// Ultimately ends up in glVertexAttribPointer.
// This specify how our vertex data should be mapped to GLSL
// attribute variable.
// Similar to QSGGeometry::defaultAttributes_TexturedPoint2D()
static const QSGGeometry::AttributeSet &qrCodeVertexAttributes()
{
  static QSGGeometry::Attribute data[] = {
    QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
    QSGGeometry::Attribute::create(1, 1, GL_FLOAT)
  };
  static QSGGeometry::AttributeSet attrs =
    { 2, sizeof(float) * 3, data };
  return attrs;
}


class QRCodeMaterialShader : public QSGMaterialShader
{
public:
  char const *const *attributeNames() const override
  {
    // Maps each QSGGeometry::Attribute to a variable name.
    static char const *const names[] = {
      "position",
      "surroundCount",
      0 };
    return names;
  }

  const char *vertexShader() const override {
    return QT_STRINGIFY(
      attribute highp vec4 position;
      attribute mediump float surroundCount;

      uniform highp mat4 matrix;
      uniform mediump float revealProgress;

      varying lowp float dotOpacity;

      void main() {
        float mediump start = 1.0 - smoothstep(
          0.0, 30.0, surroundCount);
        float mediump end = 1.0 - smoothstep(
          0.0, 30.0, surroundCount + 5.0);
        float lowp dotProgress = smoothstep(
          start, end, revealProgress);
        highp vec4 translatedPosition = position;
        translatedPosition.y +=
          dotProgress * dotProgress * 25.0 * 0.5;

        dotOpacity = 1.0 - dotProgress;
        gl_Position = matrix * translatedPosition;
      });
  }

  const char *fragmentShader() const override {
    return QT_STRINGIFY(
      varying lowp float dotOpacity;
      uniform lowp float itemOpacity;

      void main() {
        gl_FragColor =
          vec4(0.0, 0.0, 0.0, 1.0) * dotOpacity * itemOpacity;
      });
  }

  void initialize() override
  {
    QSGMaterialShader::initialize();
    m_id_matrix =
      program()->uniformLocation("matrix");
    m_id_itemOpacity =
      program()->uniformLocation("itemOpacity");
    m_id_revealProgress =
      program()->uniformLocation("revealProgress");
  }

  // This is the most important method
  void updateState(const RenderState &state,
    QSGMaterial *newMaterial,
    QSGMaterial *oldMaterial) override;

private:
  int m_id_matrix;
  int m_id_itemOpacity;
  int m_id_revealProgress;
};


class QRCodeMaterial : public QSGMaterial
{
public:
  QRCodeMaterial() {
    // We will output transparent fragments,
    // this could be turned off when revealProgress == 1
    setFlag(QSGMaterial::Blending);
    // The y translation we do in the vertex shader relies on
    // vertices using the QR code coordinate system. This tells
    // the scene graph not to merge this batch by flatening the
    // transforms to an arbitrary root.
    setFlag(QSGMaterial::RequiresFullMatrix);
  }

  QSGMaterialType *type() const {
    static QSGMaterialType type;
    return &type;
  }
  QSGMaterialShader *createShader() const {
    return new QRCodeMaterialShader;
  }
  int compare(const QSGMaterial *other) const {
    return revealProgress !=
    static_cast<const QRCodeMaterial*>(other)->revealProgress;
  }

  // Our material states to be used by our QSGMaterialShader
  float revealProgress = 0;
};


void QRCodeMaterialShader::updateState(const RenderState &state,
  QSGMaterial *newMaterial,
  QSGMaterial *)
{
  // Set uniforms values managed by the scene graph itself
  Q_ASSERT(program()->isLinked());
  if (state.isMatrixDirty())
    program()->setUniformValue(m_id_matrix,
      state.combinedMatrix());
  if (state.isOpacityDirty())
    program()->setUniformValue(m_id_itemOpacity,
      state.opacity());

  // Set our own material uniform values
  program()->setUniformValue(
    m_id_revealProgress,
    static_cast<QRCodeMaterial*>(newMaterial)->revealProgress);
}

} // namespace


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
    rootScaleNode->appendChildNode(
      new QSGSimpleRectNode(QRectF(), Qt::white));
    auto geometryNode = new QSGGeometryNode;
    geometryNode->setMaterial(new QRCodeMaterial);
    geometryNode->setFlag(QSGNode::OwnsMaterial);
    rootScaleNode->appendChildNode(geometryNode);
  }
  auto bgNode = static_cast<QSGSimpleRectNode *>(
    rootScaleNode->firstChild());
  auto geometryNode = static_cast<QSGGeometryNode *>(
    rootScaleNode->lastChild());
  auto qrCodeMaterial = static_cast<QRCodeMaterial *>(
    geometryNode->material());

  // This is unexpensive, no need for a m_revealProgressDirty
  qrCodeMaterial->revealProgress = m_revealProgress;
  // IMPORTANT: markDirty always needs to be
  // called manually when an existing material changes
  geometryNode->markDirty(QSGNode::DirtyMaterial);

  // Now check if the text changed
  if (m_textDirty) {
    m_textDirty = false;

    const Code code = getQRCodeData(m_text).value<Code>();
    int contentWidth = code.width + 2;
    bgNode->setRect(0, 0, contentWidth, contentWidth);

    auto geometry = new QSGGeometry(
      qrCodeVertexAttributes(),
      code.dots.size() * 4,
      code.dots.size() * 6);
    // The default GL_TRIANGLE_STRIP wouldn't allow
    // us to have separate non-touching squares
    geometry->setDrawingMode(GL_TRIANGLES);
    geometryNode->setGeometry(geometry);
    geometryNode->setFlag(QSGNode::OwnsGeometry);

    // Use the QRCodeVertex struct as a view to modify
    // our vertex data buffer
    auto vertexPointer =
      reinterpret_cast<QRCodeVertex*>(geometry->vertexData());
    auto indexPointer = geometry->indexDataAsUShort();

    for (int i = 0; i < code.dots.size(); ++i) {
      CodeDot dot = code.dots.at(i).value<CodeDot>();
      // + 1 dot offset for the empty edge
      float posX = dot.x + 1;
      float posY = dot.y + 1;
      quint16 startIndex = i * 4;

      // Corners
      vertexPointer[startIndex] = QRCodeVertex{
        posX, posY, float(dot.surroundCount)};
      vertexPointer[startIndex+1] = QRCodeVertex{
        posX+1, posY, float(dot.surroundCount)};
      vertexPointer[startIndex+2] = QRCodeVertex{
        posX, posY+1, float(dot.surroundCount)};
      vertexPointer[startIndex+3] = QRCodeVertex{
        posX+1, posY+1, float(dot.surroundCount)};

      // Top-left triangle
      *indexPointer++ = startIndex;
      *indexPointer++ = startIndex+1;
      *indexPointer++ = startIndex+2;
      // Bottom-right triangle
      *indexPointer++ = startIndex+2;
      *indexPointer++ = startIndex+1;
      *indexPointer++ = startIndex+3;
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

