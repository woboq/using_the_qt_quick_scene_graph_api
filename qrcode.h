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
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  Q_PROPERTY(float revealProgress READ revealProgress WRITE setRevealProgress NOTIFY revealProgressChanged)
public:
  QRCodeItem();
  QString text() const { return m_text; }
  void setText(const QString &text);
  float revealProgress() const { return m_revealProgress; }
  void setRevealProgress(float revealProgress);

  signals:
  void textChanged();
  void revealProgressChanged();

protected:
  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
  static QVariant getQRCodeData(const QString &text);
  QString m_text;
  float m_revealProgress = 0;

  bool m_textDirty = true;
};

#endif // QRCODE_H
