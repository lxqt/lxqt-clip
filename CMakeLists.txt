# CMakeLists.txt
# ------------------------------------
# An initial version of the cmake build for lxqt-clip.
# WORK IN PROGRESS. USE qmake STILL FOR NOW, PLEASE!
# Currently tested on unix only.
# --------------------------------
# usage
# 1. cd lxqt-clip/root/dir
# 2. mkdir build
# 3. cd build
# 4. cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=debug (or release, by default)
# 5. make
# 6. make install
# --------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
# from 3.1.0 to 3.6 because CMake now don't support backward compatibility for CMake < v3.5
# and in future support for 3.11 is also getting dropped !
project(lxqt-clip)


set(LXQTCLIP_VERSION 2.4.0)

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


add_definitions(-DIS_CMAKE)
add_definitions(-DLXQTCLIP_VERSION="${LXQTCLIP_VERSION}")


# source code and compilation
set (LXQTCLIP_HEADERS
    qkeysequencewidget/qkeysequencewidget.h
    qkeysequencewidget/qkeysequencewidget_p.h
    qmenuview/qmenuview.h
    qmenuview/qmenuview_p.h
    src/model.h
    src/preferencesdialog.h
    src/systray.h
    src/clipboardwrap.h
    src/dbusinterface.h
    src/signalhandler.h
)
set (LXQTCLIP_SOURCES 
    qkeysequencewidget/qkeysequencewidget.cpp
    qmenuview/qmenuview.cpp
    src/main.cpp
    src/item.cpp
    src/model.cpp
    src/preferences.cpp
    src/preferencesdialog.cpp
    src/systray.cpp
    src/clipboardwrap.cpp
    src/dbusinterface.cpp
    src/signalhandler.cpp
)

if (NOT QXT_FOUND)

    set (HAVE_QXT 1)
    set (LXQTCLIP_SOURCES ${LXQTCLIP_SOURCES} qxt/qxtglobalshortcut_x11.cpp)
    include_directories(${Qt6Gui_PRIVATE_INCLUDE_DIRS})

    if (HAVE_QXT)
        set (LXQTCLIP_SOURCES ${LXQTCLIP_SOURCES}
            qxt/qxtglobalshortcut.cpp
        )
        set (LXQTCLIP_HEADERS ${LXQTCLIP_HEADERS}
            qxt/qxtglobalshortcut.h
        )
    endif (HAVE_QXT)

endif (NOT QXT_FOUND)
FIND_PACKAGE(X11 REQUIRED)
MESSAGE(STATUS "X11_X11_LIB: ${X11_X11_LIB}")

set (LXQTCLIP_FORMS
    src/preferencesdialog.ui
)

set (LXQTCLIP_RESOURCES
    src/lxqt-clip.qrc
)

set (LXQTCLIP_TRANSLATIONS
    ts/lxqt-clip_ca.ts
    ts/lxqt-clip_cs.ts
    ts/lxqt-clip_de.ts
    ts/lxqt-clip_es.ts
    ts/lxqt-clip_ja.ts
    ts/lxqt-clip_lt.ts
    ts/lxqt-clip_pl.ts
    ts/lxqt-clip_sr.ts
    ts/lxqt-clip_sv.ts
    ts/lxqt-clip_it.ts
    ts/lxqt-clip_zh_CN.ts
    ts/lxqt-clip_pt.ts
)

QT6_WRAP_UI( LXQTCLIP_UI ${LXQTCLIP_FORMS} )
QT6_WRAP_CPP( LXQTCLIP_MOC ${LXQTCLIP_HEADERS} )
QT6_ADD_RESOURCES( LXQTCLIP_RCC ${LXQTCLIP_RESOURCES} )
QT6_ADD_TRANSLATION( LXQTCLIP_QM ${LXQTCLIP_TRANSLATIONS} )

message(STATUS "MOCs: " ${LXQTCLIP_MOC})

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


add_executable(lxqt-clip
    ${LXQTCLIP_SOURCES}
    ${LXQTCLIP_UI}
    ${LXQTCLIP_MOC}
    ${LXQTCLIP_RCC}
    ${LXQTCLIP_QM}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        Qt6::DBus
        KF6::GuiAddons
        Qt6::GuiPrivate
        ${X11_X11_LIB}
)


if (QXT_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QXT_CORE_LIB} ${QXT_GUI_LIB})
endif (QXT_FOUND)


# installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES lxqt-clip.desktop DESTINATION share/applications)
install(FILES src/icons/lxqt-clip.png DESTINATION share/icons/hicolor/128x128/apps)
install(FILES ${LXQTCLIP_QM} DESTINATION share/lxqt/lxqt-clip/translations)
add_definitions(-DTRANSLATION_DIR="${CMAKE_INSTALL_PREFIX}/share/lxqt/lxqt-clip/translations")

# merged from lxqt-common
if (ENABLE_LXQT_AUTOSTART)
    set(LXQTBT_MINIMUM_VERSION "2.0.0")
    find_package(lxqt2-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
    include(LXQtConfigVars)
    include(LXQtTranslateDesktop)
    add_subdirectory(lxqt-autostart)
endif ()
