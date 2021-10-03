set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)

add_executable(HotkeyTest
    main.cpp
    hottestwidget.cpp
    hottestwidget.ui)

find_package(Qt${QT_MAJOR} COMPONENTS Widgets REQUIRED)
target_link_libraries(HotkeyTest Qt${QT_MAJOR}::Widgets QHotkey::QHotkey)
