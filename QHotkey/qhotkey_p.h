#ifndef QHOTKEY_P_H
#define QHOTKEY_P_H

#include "qhotkey.h"
#include <QAbstractNativeEventFilter>
#include <QMultiHash>
#include <QMutex>

class QHotkeyPrivate : public QAbstractNativeEventFilter
{
public:
	QHotkeyPrivate();//singleton!!!
	~QHotkeyPrivate();

	// QAbstractNativeEventFilter interface
	bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

	QHotkey::NativeShortcut nativeShortcut(Qt::Key keycode, Qt::KeyboardModifiers modifiers);

	bool addShortcut(QHotkey *hotkey);
	bool removeShortcut(QHotkey *hotkey);//platform implement

private:
	QMutex mutex;
	QMultiHash<QHotkey::NativeShortcut, QHotkey*> shortcuts;

	void activateShortcut(QHotkey::NativeShortcut shortcut);

	quint32 nativeKeycode(Qt::Key keycode);//platform implement
	quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);//platform implement

	bool registerShortcut(QHotkey::NativeShortcut shortcut);//platform implement
	bool unregisterShortcut(QHotkey::NativeShortcut shortcut);//platform implement
};

#endif // QHOTKEY_P_H
