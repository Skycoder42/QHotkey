# QHotkey
A global shortcut/hotkey for Desktop Qt-Applications.

The QHotkey is a class that can be used to create hotkeys/global shortcuts, aka shortcuts that work everywhere, independent of the application state. This means your application can be active, inactive, minimized or not visible at all and still receive the shortcuts.

## Features
 - Works on Windows, Mac and X11
 - Easy to use, can use QKeySequence for easy shortcut input
 - Supports almost all common keys
 - Allows direct input of Key/Modifier-Combinations
 - Supports multiple QHotkey-instances for the same shortcut
 - Thread-Safe - Can be used on all threads (If the platform supports it)

## Usage
### Example
cooming soon...

### Testing
cooming soon...

### Use with your application
Just copy the `./QHotkey` folder into you application and add the line `include(./QHotkey/qhotkey.pri)` to your .pro-file. This ways all files and required libraries will automatically be added. Use `#include <QHotkey>` to access the class.

## Technical
### Requirements
 - I built it with Qt 5.5.1, but may work with earlier versions, too
 - C++11

### Known Limitations
 - Qt::Key makes no difference between normal numbers and the Numpad numbers. Windows however requires this. Thus, you can't register shortcuts for the numpad
 - Supports not all keys, but most of the common ones.
