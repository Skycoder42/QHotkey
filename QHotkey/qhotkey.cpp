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
	key(Qt::Key_unknown),
	mods(Qt::NoModifier),
	nativeHkey(),
	registered(false)
{}

QHotkey::QHotkey(const QKeySequence &sequence, bool autoRegister, QObject *parent) :
	QHotkey(parent)
{
	this->setShortcut(sequence, autoRegister);
}

QHotkey::QHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister, QObject *parent) :
	QHotkey(parent)
{
	this->setShortcut(key, modifiers, autoRegister);
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

Qt::Key QHotkey::keyCode() const
{
	return this->key;
}

Qt::KeyboardModifiers QHotkey::modifiers() const
{
	return this->mods;
}

QHotkey::NativeShortcut QHotkey::nativeShortcut() const
{
	return this->nativeHkey;
}

bool QHotkey::isRegistered() const
{
	return this->registered;
}

bool QHotkey::setShortcut(const QKeySequence &shortcut, bool autoRegister)
{
	if(shortcut.isEmpty()) {
		return this->resetShortcut();
	} else if(shortcut.count() > 1) {
		qWarning("QHotkey: Keysequences with multiple shortcuts are not allowed! "
				 "Only the first shortcut will be used!");
	}

	return this->setShortcut(Qt::Key(shortcut[0] & ~Qt::KeyboardModifierMask),
							 Qt::KeyboardModifiers(shortcut[0] & Qt::KeyboardModifierMask),
							 autoRegister);
}

bool QHotkey::setShortcut(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister)
{
	if(this->registered) {
		if(autoRegister) {
			if(!hotkeyPrivate->removeShortcut(this))
				return false;
		} else {
			qWarning("QHotkey: Can't change the shortcut while the hotkey is registered");
			return false;
		}
	}

	this->key = key;
	this->mods = modifiers;
	this->nativeHkey = hotkeyPrivate->nativeShortcut(key, modifiers);
	if(this->nativeHkey.isValid()) {
		if(autoRegister)
			return hotkeyPrivate->addShortcut(this);
		else
			return true;
	} else {
		qWarning("QHotkey: Unable to map shortcut to native keys.");
		this->key = Qt::Key_unknown;
		this->mods = Qt::NoModifier;
		this->nativeHkey = NativeShortcut();
		return false;
	}
}

bool QHotkey::resetShortcut()
{
	if(this->registered &&
	   !hotkeyPrivate->removeShortcut(this)) {
		qWarning("QHotkey: Failed to unregister shortcut");
		return false;
	}

	this->key = Qt::Key_unknown;
	this->mods = Qt::NoModifier;
	this->nativeHkey = NativeShortcut();
	return true;
}

bool QHotkey::setRegistered(bool registered)
{
	if(this->registered && !registered)
		return hotkeyPrivate->removeShortcut(this);
	else if(!this->registered && registered) {
		if(!this->nativeHkey.isValid())
			return false;
		else
			return hotkeyPrivate->addShortcut(this);
	} else
		return true;
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
	QHotkey::NativeShortcut shortcut = hotkey->nativeHkey;

	LOCKER;
	if(!this->shortcuts.contains(shortcut)) {
		if(!this->registerShortcut(shortcut))
			return false;
	}

	this->shortcuts.insert(shortcut, hotkey);
	hotkey->registered = true;
	emit hotkey->registeredChanged(true);
	return true;
}

bool QHotkeyPrivate::removeShortcut(QHotkey *hotkey)
{
	if(!hotkey->registered)
		return false;
	QHotkey::NativeShortcut shortcut = hotkey->nativeHkey;

	LOCKER;
	if(this->shortcuts.remove(shortcut, hotkey) == 0)
		return false;
	hotkey->registered = false;
	emit hotkey->registeredChanged(true);
	if(this->shortcuts.count(shortcut) == 0)
		return this->unregisterShortcut(shortcut);
	else
		return true;
}

void QHotkeyPrivate::activateShortcut(QHotkey::NativeShortcut shortcut)
{
	LOCKER;

	QMetaMethod signal = QMetaMethod::fromSignal(&QHotkey::activated);
	for(QHotkey *hkey : this->shortcuts.values(shortcut))
		signal.invoke(hkey, Qt::QueuedConnection);
}
