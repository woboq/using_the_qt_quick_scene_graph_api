#ifndef QRCODE_H
#define QRCODE_H

#include <QObject>
#include <QQuickItem>
#include <QVariant>

struct CodeDot {
  int x, y;
  int surroundCount;

  Q_PROPERTY(int x MEMBER x)
  Q_PROPERTY(int y MEMBER y)
  Q_PROPERTY(int surroundCount MEMBER surroundCount)
  Q_GADGET
};

struct Code {
  int width;
  QVariantList dots;

  Q_PROPERTY(int width MEMBER width)
  Q_PROPERTY(QVariantList dots MEMBER dots)
  Q_GADGET
};

class QRCodeItem : public QQuickItem {
  Q_OBJECT
public:
  QRCodeItem();

protected:
  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
  static QVariant getQRCodeData(const QString &text);
};

#endif // QRCODE_H
