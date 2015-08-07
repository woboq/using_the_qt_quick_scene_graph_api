#include "qrcode.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>

int main(int argc, char **argv)
{
  qmlRegisterType<QRCodeItem>("main", 1, 0, "QRCodeItem");
  QGuiApplication app{argc, argv};

  QQuickView view;
  QSurfaceFormat surfaceFormat = view.requestedFormat();
  surfaceFormat.setSamples(4);
  view.setFormat(surfaceFormat);
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.setSource(QUrl{"qrc:///main.qml"});
  view.setColor(Qt::black);
  view.show();

  return app.exec();
}
