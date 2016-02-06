# QHotkey
A global shortcut/hotkey for Desktop Qt-Applications.

The QHotkey is a class that can be used to create hotkeys/global shortcuts, aka shortcuts that work everywhere, independent of the application state. This means your application can be active, inactive, minimized or not visible at all and still receive the shortcuts.

## Status
**Still in development!!!** - Can already be used but is not ready yet.

The class works properly on all 3 OS, but still has some issues with multi-threaded usage on windows. It works on mac/X11, but I need to investigate further, if the things I'm doing are safe. If you are just using the main thread, it will work fine.

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
 - Qt::Key makes no difference between normal numbers and the Numpad numbers. Most keyboards however requires this. Thus, you can't register shortcuts for the numpad
 - Supports not all keys, but most of the common ones. There are differences between platforms. "Delete", for example, works on windows and mac, but not on X11
