CONFIG += C++11

HEADERS += $$PWD/qhotkey.h \
    $$PWD/qhotkey_p.h

SOURCES += $$PWD/qhotkey.cpp

mac {
	SOURCES += $$PWD/qhotkey_mac.cpp
	LIBS += -framework Carbon
} else:win32 {
	SOURCES += $$PWD/qhotkey_win.cpp
	LIBS += -lUser32
} else:unix {
	SOURCES += $$PWD/qhotkey_x11.cpp
	QT += x11extras
	LIBS += -lX11
}

INCLUDEPATH += $$PWD
