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

	static QHotkeyPrivate *instance();

	// QAbstractNativeEventFilter interface
	bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

	QHotkey::NativeShortcut nativeShortcut(Qt::Key keycode, Qt::KeyboardModifiers modifiers);
	bool hasShortcut(Qt::Key keycode, Qt::KeyboardModifiers modifiers);

	bool addShortcut(QHotkey *hotkey);
	bool removeShortcut(QHotkey *hotkey);

private:
	QMutex mutex;
	QMultiHash<QHotkey::NativeShortcut, QHotkey*> shortcuts;

	void activateShortcut(QHotkey::NativeShortcut shortcut);

	quint32 nativeKeycode(Qt::Key keycode);//platform implement
	quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);//platform implement

	bool registerShortcut(QHotkey::NativeShortcut shortcut);//platform implement
	bool unregisterShortcut(QHotkey::NativeShortcut shortcut);//platform implement
};

uint qHash(QHotkey::NativeShortcut key, uint seed = 0);



// ---------- QHotkey::NativeShortcut implementation ----------

inline QHotkey::NativeShortcut::NativeShortcut() :
	key(0),
	mods(0)
{}

inline QHotkey::NativeShortcut::NativeShortcut(const QHotkey::NativeShortcut &other) :
	key(other.key),
	mods(other.mods)
{}

inline QHotkey::NativeShortcut &QHotkey::NativeShortcut::operator =(const QHotkey::NativeShortcut &other)
{
	this->key = other.key;
	this->mods = other.mods;
	return (*this);
}

inline bool QHotkey::NativeShortcut::isValid() const
{
	return (this->key != 0);
}

inline bool QHotkey::NativeShortcut::operator==(const QHotkey::NativeShortcut &other) const
{
	return (this->key == other.key && this->mods == other.mods);
}

inline uint qHash(QHotkey::NativeShortcut key, uint seed)
{
	return qHash(key.key ^ key.mods, seed);
}

#endif // QHOTKEY_P_H
