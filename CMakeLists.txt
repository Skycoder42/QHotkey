cmake_minimum_required(VERSION 3.1)

project(qhotkey VERSION 1.2.2 LANGUAGES CXX)

option(QHOTKEY_EXAMPLES "Build examples" OFF)
option(QHOTKEY_INSTALL "Enable install rule" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

set(QT_MAJOR 5 CACHE STRING "Qt major version to use")

if(QT_MAJOR EQUAL 6)
    find_package(Qt${QT_MAJOR} 6.2.0 COMPONENTS Core Gui REQUIRED)
else()
    find_package(Qt${QT_MAJOR} COMPONENTS Core Gui REQUIRED)
endif()

add_library(qhotkey QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)
target_link_libraries(qhotkey PUBLIC Qt${QT_MAJOR}::Core)
target_link_libraries(qhotkey PRIVATE Qt${QT_MAJOR}::Gui)

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif(WIN32)
    target_sources(qhotkey PRIVATE QHotkey/qhotkey_win.cpp)
else()
    find_package(X11 REQUIRED)
    if(QT_MAJOR GREATER_EQUAL 6)
        target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})
    else()
        find_package(Qt${QT_MAJOR} COMPONENTS X11Extras REQUIRED)
        target_link_libraries(qhotkey
            PRIVATE
                ${X11_LIBRARIES}
                Qt${QT_MAJOR}::X11Extras)
    endif()

    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE QHotkey/qhotkey_x11.cpp)
endif()

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
