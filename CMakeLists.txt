cmake_minimum_required(VERSION 3.1)

project(qhotkey
    VERSION 1.5.0
    DESCRIPTION "Global hotkey library for Qt software"
    HOMEPAGE_URL "https://skycoder42.github.io/QHotkey/"
    LANGUAGES CXX)

option(QHOTKEY_EXAMPLES "Build examples" OFF)
option(QHOTKEY_INSTALL "Enable install rule" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

if(NOT QT_DEFAULT_MAJOR_VERSION)
    set(QT_DEFAULT_MAJOR_VERSION 5 CACHE STRING "Qt version to use (5 or 6), defaults to 5")
endif()

if(QT_DEFAULT_MAJOR_VERSION EQUAL 6)
    find_package(Qt${QT_DEFAULT_MAJOR_VERSION} 6.2.0 COMPONENTS Core Gui REQUIRED)
else()
    find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS Core Gui REQUIRED)
endif()

# General settings
set(CPACK_PACKAGE_VENDOR "Skycoder42")
set(CPACK_PACKAGE_CONTACT "Shatur")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
# CPACK: DEB Specific Settings
set(CPACK_DEBIAN_PACKAGE_NAME "libqhotkey")
set(CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
# Set dependencies
if(QT_DEFAULT_MAJOR_VERSION EQUAL 6)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6x11extras6 (>= 6.2.0)")
else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5x11extras5 (>= 5.15.2)")
endif()
include(CPack)

add_library(qhotkey QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)
target_link_libraries(qhotkey PUBLIC Qt${QT_DEFAULT_MAJOR_VERSION}::Core Qt${QT_DEFAULT_MAJOR_VERSION}::Gui)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(qhotkey PRIVATE QHOTKEY_LIBRARY)
    target_compile_definitions(qhotkey PUBLIC QHOTKEY_SHARED)
endif()

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif(WIN32)
    target_sources(qhotkey PRIVATE QHotkey/qhotkey_win.cpp)
else()
    find_package(X11 REQUIRED)
    if(QT_DEFAULT_MAJOR_VERSION GREATER_EQUAL 6)
        target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})
    else()
        find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS X11Extras REQUIRED)
        target_link_libraries(qhotkey
            PRIVATE
                ${X11_LIBRARIES}
                Qt${QT_DEFAULT_MAJOR_VERSION}::X11Extras)
    endif()

    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE QHotkey/qhotkey_x11.cpp)
endif()

include(GNUInstallDirs)

target_include_directories(qhotkey
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QHotkey>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(CMakePackageConfigHelpers)

set_target_properties(qhotkey PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    INTERFACE_QHotkey_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
    COMPATIBLE_INTERFACE_STRING QHotkey_MAJOR_VERSION)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QHotkeyConfigVersion.cmake
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion)

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
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QHotkeyConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR})
    install(EXPORT QHotkeyConfig DESTINATION ${INSTALL_CONFIGDIR})

    export(TARGETS qhotkey FILE QHotkeyConfig.cmake)
endif()
