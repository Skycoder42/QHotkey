#ifndef QHOTKEY_H
#define QHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QPair>
#include <QLoggingCategory>

//! A class to define global, systemwide Hotkeys
class QHotkey : public QObject
{
	Q_OBJECT
	friend class QHotkeyPrivate;

	//! Specifies whether this hotkey is currently registered or not
	Q_PROPERTY(bool registered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)
	//! Holds the shortcut this hotkey will be triggered on
	Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut RESET resetShortcut)

public:
	//! Defines shortcut with native keycodes
	typedef QPair<quint32, quint32> NativeShortcut;

	//! Constructor
	explicit QHotkey(QObject *parent = 0);
	//! Constructs a hotkey with a shortcut and optionally registers it
	explicit QHotkey(const QKeySequence &shortcut, bool autoRegister = true, QObject *parent = 0);
	//! Constructs a hotkey with a key and modifiers and optionally registers it
	explicit QHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister = true, QObject *parent = 0);
	//! Destructor
	~QHotkey();

	//! Test if a certain key with modifiers is currently registered somewhere or not
	static bool isKeyCaptured(Qt::Key key, Qt::KeyboardModifiers modifiers);

	//! READ-Accessor for QHotkey::registered
	bool isRegistered() const;
	//! READ-Accessor for QHotkey::shortcut
	QKeySequence shortcut() const;
	//! The key-code of the current shortcut
	Qt::Key keyCode() const;
	//! The modifiers of the current shortcut
	Qt::KeyboardModifiers modifiers() const;

public slots:
	//! WRITE-Accessor for QHotkey::registered
	bool setRegistered(bool registered);

	//! WRITE-Accessor for QHotkey::shortcut
	bool setShortcut(const QKeySequence& shortcut, bool autoRegister = true);
	//! WRITE-Accessor for QHotkey::shortcut
	bool setShortcut(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister = true);
	//! RESET-Accessor for QHotkey::shortcut
	bool resetShortcut();

signals:
	//! Will be emitted if the shortcut is pressed
	void activated();

	//! NOTIFY-Accessor for QHotkey::registered
	void registeredChanged(bool registered);

private:
	Qt::Key key;
	Qt::KeyboardModifiers mods;

	NativeShortcut nativeShortcut;
	bool registered;
};

Q_DECLARE_LOGGING_CATEGORY(logQHotkey)

#endif // QHOTKEY_H
