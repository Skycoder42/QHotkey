#include "qhotkey.h"
#include "qhotkey_p.h"
#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QMetaMethod>
#include <QThread>
#include <QDebug>

Q_LOGGING_CATEGORY(logQHotkey, "QHotkey")

QHotkey::QHotkey(QObject *parent) :
	QObject(parent),
	key(Qt::Key_unknown),
	mods(Qt::NoModifier),
	nativeShortcut(),
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
		QHotkeyPrivate::instance()->removeShortcut(this);
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

bool QHotkey::isRegistered() const
{
	return this->registered;
}

bool QHotkey::setShortcut(const QKeySequence &shortcut, bool autoRegister)
{
	if(shortcut.isEmpty()) {
		return this->resetShortcut();
	} else if(shortcut.count() > 1) {
		qCWarning(logQHotkey, "Keysequences with multiple shortcuts are not allowed! "
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
			if(!QHotkeyPrivate::instance()->removeShortcut(this))
				return false;
		} else
			return false;
	}

	if(key == Qt::Key_unknown) {
		this->key = Qt::Key_unknown;
		this->mods = Qt::NoModifier;
		this->nativeShortcut = NativeShortcut();
		return true;
	}

	this->key = key;
	this->mods = modifiers;
	this->nativeShortcut = QHotkeyPrivate::instance()->nativeShortcut(key, modifiers);
	if(QHotkeyPrivate::testValid(this->nativeShortcut)) {
		if(autoRegister)
			return QHotkeyPrivate::instance()->addShortcut(this);
		else
			return true;
	} else {
		qCWarning(logQHotkey) << "Unable to map shortcut to native keys. Key:" << key << "Modifiers:" << modifiers;
		this->key = Qt::Key_unknown;
		this->mods = Qt::NoModifier;
		this->nativeShortcut = NativeShortcut();
		return false;
	}
}

bool QHotkey::resetShortcut()
{
	if(this->registered &&
	   !QHotkeyPrivate::instance()->removeShortcut(this)) {
		return false;
	}

	this->key = Qt::Key_unknown;
	this->mods = Qt::NoModifier;
	this->nativeShortcut = NativeShortcut();
	return true;
}

bool QHotkey::setRegistered(bool registered)
{
	if(this->registered && !registered)
		return QHotkeyPrivate::instance()->removeShortcut(this);
	else if(!this->registered && registered) {
		if(!QHotkeyPrivate::testValid(this->nativeShortcut))
			return false;
		else
			return QHotkeyPrivate::instance()->addShortcut(this);
	} else
		return true;
}



// ---------- QHotkeyPrivate implementation ----------

QHotkeyPrivate::QHotkeyPrivate() :
	shortcuts()
{
	Q_ASSERT_X(qApp, Q_FUNC_INFO, "QHotkey requires QCoreApplication to be instantiated");
	qApp->eventDispatcher()->installNativeEventFilter(this);
}

QHotkeyPrivate::~QHotkeyPrivate()
{
	if(!this->shortcuts.isEmpty())
		qCWarning(logQHotkey, "QHotkeyPrivate destroyed with registered shortcuts!");
	if(qApp && qApp->eventDispatcher())
		qApp->eventDispatcher()->removeNativeEventFilter(this);
}

QHotkey::NativeShortcut QHotkeyPrivate::nativeShortcut(Qt::Key keycode, Qt::KeyboardModifiers modifiers)
{
	Qt::ConnectionType conType = (QThread::currentThread() == this->thread() ?
									  Qt::DirectConnection :
									  Qt::BlockingQueuedConnection);
	QHotkey::NativeShortcut res;
	if(!QMetaObject::invokeMethod(this, "nativeShortcutInvoked", conType,
								  Q_RETURN_ARG(QHotkey::NativeShortcut, res),
								  Q_ARG(Qt::Key, keycode),
								  Q_ARG(Qt::KeyboardModifiers, modifiers))) {
		return QHotkey::NativeShortcut();
	} else
		return res;
}

bool QHotkeyPrivate::addShortcut(QHotkey *hotkey)
{
	if(hotkey->registered)
		return false;

	Qt::ConnectionType conType = (QThread::currentThread() == this->thread() ?
									  Qt::DirectConnection :
									  Qt::BlockingQueuedConnection);
	bool res = false;
	if(!QMetaObject::invokeMethod(this, "addShortcutInvoked", conType,
								  Q_RETURN_ARG(bool, res),
								  Q_ARG(QHotkey*, hotkey))) {
		return false;
	} else {
		if(res)
			emit hotkey->registeredChanged(true);
		return res;
	}
}

bool QHotkeyPrivate::removeShortcut(QHotkey *hotkey)
{
	if(!hotkey->registered)
		return false;

	Qt::ConnectionType conType = (QThread::currentThread() == this->thread() ?
									  Qt::DirectConnection :
									  Qt::BlockingQueuedConnection);
	bool res = false;
	if(!QMetaObject::invokeMethod(this, "removeShortcutInvoked", conType,
								  Q_RETURN_ARG(bool, res),
								  Q_ARG(QHotkey*, hotkey))) {
		return false;
	} else {
		if(res)
			emit hotkey->registeredChanged(false);
		return res;
	}
}

void QHotkeyPrivate::activateShortcut(QHotkey::NativeShortcut shortcut)
{
	QMetaMethod signal = QMetaMethod::fromSignal(&QHotkey::activated);
	for(QHotkey *hkey : this->shortcuts.values(shortcut))
		signal.invoke(hkey, Qt::QueuedConnection);
}

bool QHotkeyPrivate::addShortcutInvoked(QHotkey *hotkey)
{
	QHotkey::NativeShortcut shortcut = hotkey->nativeShortcut;

	if(!this->shortcuts.contains(shortcut)) {
		if(!this->registerShortcut(shortcut))
			return false;
	}

	this->shortcuts.insert(shortcut, hotkey);
	hotkey->registered = true;
	return true;
}

bool QHotkeyPrivate::removeShortcutInvoked(QHotkey *hotkey)
{
	QHotkey::NativeShortcut shortcut = hotkey->nativeShortcut;

	if(this->shortcuts.remove(shortcut, hotkey) == 0)
		return false;
	hotkey->registered = false;
	emit hotkey->registeredChanged(true);
	if(this->shortcuts.count(shortcut) == 0)
		return this->unregisterShortcut(shortcut);
	else
		return true;
}
