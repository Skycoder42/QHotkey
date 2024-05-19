TEMPLATE = subdirs

macx: CONFIG += x86_64

SUBDIRS += \
	HotkeyTest \
    QHotkey

DISTFILES += README.md \
	LICENSE \
	doc/qhotkey.doxy \
	doc/qhotkey.dox
