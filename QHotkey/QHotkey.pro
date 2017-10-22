TARGET = QHotkey
CONFIG += dll
VERSION = 1.0
TEMPLATE = lib
include(../qhotkey.pri)
headers.files = $$HEADERS QHotkey
headers.path = /$(DESTDIR)$$[QT_INSTALL_HEADERS]
target.path = /$(DESTDIR)$$[QT_INSTALL_LIBS]
INSTALLS += target headers

