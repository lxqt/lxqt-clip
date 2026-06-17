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

cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
project(lxqt-clip)

set(LXQTCLIP_VERSION 2.4.0)
set(LXQT_MINIMUM_VERSION "2.4.0")

option(USE_SYSTEM_QXT "Use system Qxt Library for global shortcuts; off for Qt6" OFF)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(lxqt ${LXQT_MINIMUM_VERSION} REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets DBus LinguistTools)
find_package(KF6GuiAddons REQUIRED)
find_package(X11 REQUIRED)

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

include(LXQtConfigVars)
include(LXQtTranslateDesktop)
include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

remove_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_FOREACH
)

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

set (LXQTCLIP_FORMS
    src/preferencesdialog.ui
)

set (LXQTCLIP_RESOURCES
    src/lxqt-clip.qrc
)

# Translations **********************************
lxqt_translate_ts(LXQTCLIP_QM
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${LXQTCLIP_HEADERS}
        ${LXQTCLIP_SOURCES}
        ${LXQTCLIP_FORMS}
    INSTALL_DIR
        "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
)
lxqt_app_translation_loader(LXQTCLIP_QM_LOADER ${PROJECT_NAME})

lxqt_translate_desktop(LXQTCLIP_DESKTOP_FILE
    SOURCES
        lxqt-clip.desktop.in
    INSTALL_DIR
        share/applications
    COMPONENT
        Runtime
    USE_YAML
)
add_custom_target(lxqtclip_desktop_file_target ALL DEPENDS ${LXQTCLIP_DESKTOP_FILE})

lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
        lxqt-clip-autostart.desktop.in
    INSTALL_DIR
        "${LXQT_ETC_XDG_DIR}/autostart"
    COMPONENT
        Runtime
    USE_YAML
)
add_custom_target(autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
#************************************************

QT6_WRAP_UI( LXQTCLIP_UI ${LXQTCLIP_FORMS} )
QT6_WRAP_CPP( LXQTCLIP_MOC ${LXQTCLIP_HEADERS} )
QT6_ADD_RESOURCES( LXQTCLIP_RCC ${LXQTCLIP_RESOURCES} )

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
    ${LXQTCLIP_QM_LOADER}
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        Qt6::DBus
        KF6::GuiAddons
        Qt6::GuiPrivate
        X11::X11
        lxqt
)

if (QXT_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QXT_CORE_LIB} ${QXT_GUI_LIB})
endif (QXT_FOUND)

# installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES src/icons/lxqt-clip.png DESTINATION share/icons/hicolor/128x128/apps)
