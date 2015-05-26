#include "qrcode.h"

#include <qrencode.h>

QVariant QRCodeHelper::getQRCodeData(const QString &text)
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

