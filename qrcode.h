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
