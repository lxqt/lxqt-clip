# CMakeLists.txt
# ------------------------------------
# An initial version of the cmake build for qlipper.
# WORK IN PROGRESS. USE qmake STILL FOR NOW, PLEASE!
# Currently tested on unix only.
# --------------------------------
# usage
# 1. cd qlipper/root/dir
# 2. mkdir build
# 3. cd build
# 4. cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=debug (or release, by default)
# 5. make
# 6. make install
# --------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
# from 3.1.0 to 3.6 because CMake now don't support backward compatibility for CMake < v3.5
# and in future support for 3.11 is also getting dropped !
project(qlipper)


set(QLIPPER_VERSION 6.1.0)

option(ENABLE_LXQT_AUTOSTART "Enables autostart for LXQt" OFF)
option(USE_SYSTEM_QXT "Use system Qxt Library for global shortcuts; off for Qt6" OFF)


LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus LinguistTools)
find_package(KF6GuiAddons REQUIRED)

# Qt 6.10: Usage of a private Qt module Foo now requires a call to find_package(Qt6 COMPONENTS FooPrivate) to make the Qt6::FooPrivate target available.
if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
    find_package(Qt6 REQUIRED COMPONENTS GuiPrivate)
endif()

if (USE_SYSTEM_QXT)
    find_package(Qxt)
    message(STATUS "Use system Qxt Library: " ${USE_SYSTEM_QXT})
    if (USE_SYSTEM_QXT AND NOT QXT_FOUND)
        message(FATAL_ERROR "System Qxt not found")
    endif ()
endif()


set(EXE_NAME "qlipper")


add_definitions(-DIS_CMAKE)
add_definitions(-DQLIPPER_VERSION="${QLIPPER_VERSION}")


# source code and compilation
set (QLIPPER_HEADERS
    qkeysequencewidget/qkeysequencewidget.h
    qkeysequencewidget/qkeysequencewidget_p.h
    qmenuview/qmenuview.h
    qmenuview/qmenuview_p.h
    src/qlippermodel.h
    src/qlipperpreferencesdialog.h
    src/qlippersystray.h
    src/clipboardwrap.h
    src/qlipperdbusinterface.h
)
set (QLIPPER_SOURCES 
    qkeysequencewidget/qkeysequencewidget.cpp
    qmenuview/qmenuview.cpp
    src/main.cpp
    src/qlipperitem.cpp
    src/qlippermodel.cpp
    src/qlipperpreferences.cpp
    src/qlipperpreferencesdialog.cpp
    src/qlippersystray.cpp
    src/clipboardwrap.cpp
    src/qlipperdbusinterface.cpp
)
if (UNIX)
    list(APPEND QLIPPER_HEADERS src/signalhandler.h)
    list(APPEND QLIPPER_SOURCES src/signalhandler.cpp)
endif (UNIX)

if (NOT QXT_FOUND)

    set (HAVE_QXT 1)
    set (QLIPPER_SOURCES ${QLIPPER_SOURCES} qxt/qxtglobalshortcut_x11.cpp)
    include_directories(${Qt6Gui_PRIVATE_INCLUDE_DIRS})

    if (HAVE_QXT)
        set (QLIPPER_SOURCES ${QLIPPER_SOURCES}
            qxt/qxtglobalshortcut.cpp
        )
        set (QLIPPER_HEADERS ${QLIPPER_HEADERS}
            qxt/qxtglobalshortcut.h
        )
    endif (HAVE_QXT)

endif (NOT QXT_FOUND)
FIND_PACKAGE(X11 REQUIRED)
MESSAGE(STATUS "X11_X11_LIB: ${X11_X11_LIB}")

set (QLIPPER_FORMS
    src/qlipperpreferencesdialog.ui
)

set (QLIPPER_RESOURCES
    src/qlipper.qrc
)

set (QLIPPER_TRANSLATIONS
    ts/qlipper_ca.ts
    ts/qlipper_cs.ts
    ts/qlipper_de.ts
    ts/qlipper_es.ts
    ts/qlipper_ja.ts
    ts/qlipper_lt.ts
    ts/qlipper_pl.ts
    ts/qlipper_sr.ts
    ts/qlipper_sv.ts
    ts/qlipper_it.ts
    ts/qlipper_zh_CN.ts
    ts/qlipper_pt.ts
)

QT6_WRAP_UI( QLIPPER_UI ${QLIPPER_FORMS} )
QT6_WRAP_CPP( QLIPPER_MOC ${QLIPPER_HEADERS} )
QT6_ADD_RESOURCES( QLIPPER_RCC ${QLIPPER_RESOURCES} )
QT6_ADD_TRANSLATION( QLIPPER_QM ${QLIPPER_TRANSLATIONS} )

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/qkeysequencewidget
    ${CMAKE_CURRENT_SOURCE_DIR}/qxt
    ${CMAKE_CURRENT_SOURCE_DIR}/qmenuview
)
if (NOT QXT_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qxt)
endif (NOT QXT_FOUND)


add_executable( ${EXE_NAME}
    ${QLIPPER_SOURCES}
    ${QLIPPER_UI}
    ${QLIPPER_MOC}
    ${QLIPPER_RCC}
    ${QLIPPER_QM}
)
set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries( ${EXE_NAME}
    PRIVATE
        Qt6::Widgets
        Qt6::DBus
        KF6::GuiAddons
        Qt6::GuiPrivate
        ${X11_X11_LIB}
)


if (QXT_FOUND)
    target_link_libraries(${EXE_NAME} ${QXT_CORE_LIB} ${QXT_GUI_LIB})
endif (QXT_FOUND)


# installation
install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
install(FILES qlipper.desktop DESTINATION share/applications)
install(FILES src/icons/qlipper.png DESTINATION share/icons/hicolor/128x128/apps)
install(FILES ${QLIPPER_QM} DESTINATION share/qlipper/translations)
add_definitions(-DTRANSLATION_DIR="${CMAKE_INSTALL_PREFIX}/share/qlipper/translations")

# merged from lxqt-common
if (ENABLE_LXQT_AUTOSTART)
    set(LXQTBT_MINIMUM_VERSION "2.0.0")
    find_package(lxqt2-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
    include(LXQtConfigVars)
    include(LXQtTranslateDesktop)
    add_subdirectory(lxqt-autostart)
endif ()
