#ifndef QHOTKEY_H
#define QHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QPair>

class QHotkey : public QObject
{
	Q_OBJECT
	friend class QHotkeyPrivate;
	friend class QHotkeyPrivateNative;

	Q_PROPERTY(bool registered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)
	Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut RESET resetShortcut)

public:
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
	struct NativeShortcut {
		quint32 key;
		quint32 mods;

		inline NativeShortcut();
		inline NativeShortcut(quint32 key, quint32 mods);
		inline NativeShortcut(const NativeShortcut &other);

		inline bool isValid() const;

		inline NativeShortcut &operator =(const NativeShortcut &other);
		inline bool operator==(const NativeShortcut &other) const;
	};

	friend uint qHash(QHotkey::NativeShortcut,uint);

	Qt::Key key;
	Qt::KeyboardModifiers mods;

	NativeShortcut nativeShortcut;
	bool registered;
};

#endif // QHOTKEY_H
