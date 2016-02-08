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
	typedef QPair<quint32, quint32> NativeShortcut;

	explicit QHotkey(QObject *parent = 0);
	explicit QHotkey(const QKeySequence &shortcut, bool autoRegister = true, QObject *parent = 0);
	explicit QHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister = true, QObject *parent = 0);
	~QHotkey();

	static bool isKeyCaptured(Qt::Key key, Qt::KeyboardModifiers modifiers);

	bool isRegistered() const;
	QKeySequence shortcut() const;
	Qt::Key keyCode() const;
	Qt::KeyboardModifiers modifiers() const;

public slots:
	bool setRegistered(bool registered);

	bool setShortcut(const QKeySequence& shortcut, bool autoRegister = true);
	bool setShortcut(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister = true);
	bool resetShortcut();

signals:
	void activated();

	void registeredChanged(bool registered);

private:
	Qt::Key key;
	Qt::KeyboardModifiers mods;

	NativeShortcut nativeShortcut;
	bool registered;
};

Q_DECLARE_LOGGING_CATEGORY(logQHotkey)

#endif // QHOTKEY_H
