set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(QT_MAJOR GREATER_EQUAL 6)
    qt_wrap_ui(test_UI_HEADERS hottestwidget.ui)
    qt_wrap_cpp(test_MOC_HEADERS hottestwidget.h)
else()
    qt5_wrap_ui(test_UI_HEADERS hottestwidget.ui)
    qt5_wrap_cpp(test_MOC_HEADERS hottestwidget.h)
endif()

add_executable(HotkeyTest
    main.cpp
    hottestwidget.cpp
    ${test_UI_HEADERS}
    ${test_MOC_HEADERS})
target_link_libraries(HotkeyTest Qt${QT_MAJOR}::Widgets qhotkey)
target_include_directories(HotkeyTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
