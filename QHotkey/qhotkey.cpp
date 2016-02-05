#include "qhotkey.h"
#include "qhotkey_p.h"
#include <QGlobalStatic>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QMetaMethod>

Q_GLOBAL_STATIC(QHotkeyPrivate, hotkeyPrivate)
#define LOCKER QMutexLocker locker(&this->mutex)

QHotkey::QHotkey(QObject *parent) :
	QObject(parent),
	enabled(true),
	key(Qt::Key_unknown),
	mods(Qt::NoModifier),
	nativeShortcut(),
	registered(false)
{}

QHotkey::QHotkey(const QKeySequence &sequence, QObject *parent) :
	QHotkey(parent)
{
	this->setShortcut(sequence);
}

QHotkey::~QHotkey()
{
	if(this->registered)
		hotkeyPrivate->removeShortcut(this);
}

QKeySequence QHotkey::shortcut() const
{
	if(this->key == Qt::Key_unknown)
		return QKeySequence();
	else
		return QKeySequence(this->key | this->mods);
}

bool QHotkey::setShortcut(const QKeySequence &shortcut)
{
	if(this->registered) {
		if(!hotkeyPrivate->removeShortcut(this)) {
			qWarning("QHotkey: Failed to unset previous shortcut. Cannot set new one.");
			return false;
		}
	}

	if(shortcut.isEmpty()) {
		this->key = Qt::Key_unknown;
		this->mods = Qt::NoModifier;
		this->nativeShortcut = NativeShortcut();
		return true;
	} else if(shortcut.count() > 1) {
		qWarning("QHotkey: Keysequences with multiple shortcuts are not allowed! "
				 "Only the first shortcut will be used!");
	}

	this->key = Qt::Key(shortcut[0] & ~Qt::KeyboardModifierMask);
	this->mods = Qt::KeyboardModifiers(shortcut[0] & Qt::KeyboardModifierMask);
	this->nativeShortcut = hotkeyPrivate->nativeShortcut(this->key, this->mods);
	if(this->nativeShortcut.isValid())
		return hotkeyPrivate->addShortcut(this);
	else {
		qWarning("QHotkey: Unable to map shortcut to native keys.");
		this->key = Qt::Key_unknown;
		this->mods = Qt::NoModifier;
		this->nativeShortcut = NativeShortcut();
		return false;
	}
}

bool QHotkey::isEnabled() const
{
	return this->enabled;
}

bool QHotkey::isRegistered() const
{
	return this->registered;
}

void QHotkey::setEnabled(bool enabled)
{
	this->enabled = enabled;
}


// ---------- QHotkey::NativeShortcut implementation ----------

QHotkey::NativeShortcut::NativeShortcut() :
	key(0),
	mods(0)
{}

QHotkey::NativeShortcut::NativeShortcut(const QHotkey::NativeShortcut &other) :
	key(other.key),
	mods(other.mods)
{}

QHotkey::NativeShortcut &QHotkey::NativeShortcut::operator =(const QHotkey::NativeShortcut &other)
{
	this->key = other.key;
	this->mods = other.mods;
	return (*this);
}

bool QHotkey::NativeShortcut::isValid() const
{
	return (this->key != 0);
}

bool QHotkey::NativeShortcut::operator==(const QHotkey::NativeShortcut &other) const
{
	return (this->key == other.key && this->mods == other.mods);
}

uint qHash(QHotkey::NativeShortcut key, uint seed)
{
	return qHash(key.key ^ key.mods, seed);
}



// ---------- QHotkeyPrivate implementation ----------

QHotkeyPrivate::QHotkeyPrivate() :
	mutex(QMutex::NonRecursive),
	shortcuts()
{
	Q_ASSERT_X(qApp, Q_FUNC_INFO, "QHotkey requires QCoreApplication to be instantiated");
	qApp->eventDispatcher()->installNativeEventFilter(this);
}

QHotkeyPrivate::~QHotkeyPrivate()
{
	if(!this->shortcuts.isEmpty()) {
		qWarning("QHotkey: QHotkeyPrivate destroyed with registered shortcuts!");
		for(QHotkey::NativeShortcut shortcut : this->shortcuts.keys())
			this->unregisterShortcut(shortcut);
	}
	if(qApp && qApp->eventDispatcher())
		qApp->eventDispatcher()->removeNativeEventFilter(this);
}

QHotkey::NativeShortcut QHotkeyPrivate::nativeShortcut(Qt::Key keycode, Qt::KeyboardModifiers modifiers) {
	QHotkey::NativeShortcut shortcut;
	shortcut.key = nativeKeycode(keycode);
	shortcut.mods = nativeModifiers(modifiers);
	return shortcut;
}

bool QHotkeyPrivate::addShortcut(QHotkey *hotkey)
{
	if(hotkey->registered)
		return false;
	QHotkey::NativeShortcut shortcut = hotkey->nativeShortcut;

	LOCKER;
	if(!this->shortcuts.contains(shortcut)) {
		if(!this->registerShortcut(shortcut))
			return false;
	}

	this->shortcuts.insert(shortcut, hotkey);
	hotkey->registered = true;
	return true;
}

bool QHotkeyPrivate::removeShortcut(QHotkey *hotkey)
{
	if(!hotkey->registered)
		return false;
	QHotkey::NativeShortcut shortcut = hotkey->nativeShortcut;

	LOCKER;
	if(this->shortcuts.remove(shortcut, hotkey) == 0)
		return false;
	hotkey->registered = false;
	if(this->shortcuts.count(shortcut) == 0)
		return this->unregisterShortcut(shortcut);
	else
		return true;
}

void QHotkeyPrivate::activateShortcut(QHotkey::NativeShortcut shortcut)
{
	LOCKER;

	QMetaMethod signal = QMetaMethod::fromSignal(&QHotkey::activated);
	for(QHotkey *hkey : this->shortcuts.values(shortcut)) {
		if(hkey->isEnabled())
			signal.invoke(hkey, Qt::QueuedConnection);
	}
}
