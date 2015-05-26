TEMPLATE = app

QT += quick
CONFIG += c++11
CONFIG -= app_bundle

CONFIG += link_pkgconfig
PKGCONFIG += libqrencode

# Input
SOURCES += \
    main.cpp \
    qrcode.cpp

HEADERS += \
    qrcode.h

RESOURCES += qrcode-animation.qrc
