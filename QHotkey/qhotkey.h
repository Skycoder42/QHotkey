#ifndef QHOTKEY_H
#define QHOTKEY_H

#include <QObject>
#include <QKeySequence>
#include <QPair>

class QHotkey : public QObject
{
	Q_OBJECT
	friend class QHotkeyPrivate;

	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
	Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut RESET reset)

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
	explicit QHotkey(const QKeySequence &shortcut, QObject *parent = 0);
	~QHotkey();

	QKeySequence shortcut() const;
	bool setShortcut(const QKeySequence& shortcut);
	bool isEnabled() const;
	bool isRegistered() const;

public slots:
	void setEnabled(bool enabled = true);
	inline void setDisabled(bool disabled = true) {
		this->setEnabled(!disabled);
	}

	inline void reset() {
		this->setShortcut(QKeySequence());
	}

signals:
	void activated();

private:
	bool enabled;
	Qt::Key key;
	Qt::KeyboardModifiers mods;

	NativeShortcut nativeShortcut;
	bool registered;
};

uint qHash(QHotkey::NativeShortcut key, uint seed = 0);

#endif // QHOTKEY_H
