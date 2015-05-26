#ifndef QRCODE_H
#define QRCODE_H

#include <QObject>
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


class QRCodeHelper : public QObject
{
  Q_OBJECT
public:
  Q_INVOKABLE static QVariant getQRCodeData(const QString &text);
};

#endif // QRCODE_H
