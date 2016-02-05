#ifndef QHOTKEY_H
#define QHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QPair>

class QHotkey : public QObject
{
	Q_OBJECT
	friend class QHotkeyPrivate;

	Q_PROPERTY(bool registered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)
	Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut RESET resetShortcut)

public:
	struct NativeShortcut {
		quint32 key;
		quint32 mods;

		NativeShortcut();
		NativeShortcut(const NativeShortcut &other);

		bool isValid() const;

		NativeShortcut &operator =(const NativeShortcut &other);
		bool operator==(const NativeShortcut &other) const;
	};

	explicit QHotkey(QObject *parent = 0);
	explicit QHotkey(const QKeySequence &shortcut, bool autoRegister = true, QObject *parent = 0);
	explicit QHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers, bool autoRegister = true, QObject *parent = 0);
	~QHotkey();

	bool isRegistered() const;
	QKeySequence shortcut() const;
	Qt::Key keyCode() const;
	Qt::KeyboardModifiers modifiers() const;
	NativeShortcut nativeShortcut() const;

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

	NativeShortcut nativeHkey;
	bool registered;
};

uint qHash(QHotkey::NativeShortcut key, uint seed = 0);

#endif // QHOTKEY_H
