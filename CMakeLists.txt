cmake_minimum_required(VERSION 3.1)

project(qhotkey VERSION 1.2.2 LANGUAGES CXX)

option(QHOTKEY_EXAMPLES "Build examples" OFF)
option(QHOTKEY_INSTALL "Enable install rule" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(QT_MAJOR 5 CACHE STRING "Qt major version to use")

if(QT_MAJOR EQUAL 6)
    find_package(Qt${QT_MAJOR} 6.2.0 COMPONENTS Core Widgets REQUIRED)
else()
    find_package(Qt${QT_MAJOR} COMPONENTS Core Widgets REQUIRED)
endif()

if(QT_MAJOR GREATER_EQUAL 6)
    qt_wrap_cpp(MOC_HEADERS
        QHotkey/qhotkey.h
        QHotkey/qhotkey_p.h)
else()
    qt5_wrap_cpp(MOC_HEADERS
        QHotkey/qhotkey.h
        QHotkey/qhotkey_p.h)
endif()

set(LIBS
    Qt${QT_MAJOR}::Core
    Qt${QT_MAJOR}::Widgets)

set(SRC_FILES
    QHotkey/qhotkey.cpp)

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    set(SRC_FILES ${SRC_FILES} QHotkey/qhotkey_mac.cpp)
    set(LIBS ${LIBS} ${CARBON_LIBRARY})
elseif(WIN32)
    set(SRC_FILES ${SRC_FILES} QHotkey/qhotkey_win.cpp)
else()
    find_package(X11 REQUIRED)
    if(QT_MAJOR GREATER_EQUAL 6)
        set(LIBS ${LIBS} ${X11_LIBRARIES})
    else()
        find_package(Qt${QT_MAJOR}X11Extras REQUIRED)
        set(LIBS ${LIBS} ${X11_LIBRARIES} Qt${QT_MAJOR}::X11Extras)
    endif()

    include_directories(${X11_INCLUDE_DIR})
    set(SRC_FILES ${SRC_FILES} QHotkey/qhotkey_x11.cpp)
endif()

add_library(qhotkey ${SRC_FILES} ${MOC_HEADERS})
add_library(QHotkey::QHotkey ALIAS qhotkey)
target_link_libraries(qhotkey ${LIBS})

include(GNUInstallDirs)

target_include_directories(qhotkey
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QHotkey>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(qhotkey PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

if(QHOTKEY_EXAMPLES)
    add_subdirectory(HotkeyTest)
endif()

if(QHOTKEY_INSTALL)
    set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/QHotkey)

    install(
        TARGETS qhotkey EXPORT QHotkeyConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/QHotkey/qhotkey.h
        ${CMAKE_CURRENT_SOURCE_DIR}/QHotkey/QHotkey
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT QHotkeyConfig DESTINATION ${INSTALL_CONFIGDIR})

    export(TARGETS qhotkey FILE QHotkeyConfig.cmake)
endif()
