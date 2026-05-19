cmake_minimum_required(VERSION 3.5)

project(melodeon VERSION 0.6.0 LANGUAGES CXX)
set(FREEDESKTOP_NAME "io.github.cdrummond.${PROJECT_NAME}")
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "DEB;RPM;TBZ2")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LMS Controller (WebApp wrapper for MaterialSkin)")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_PACKAGE_CONTACT "Craig Drummond <craig.p.drummond@gmail.com>")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES  "/.svn/;/.git/;.gitignore;.github/;.project;CMakeLists.txt.user;/screenshots/;/build/")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_STRIP_FILES YES)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

find_package(QT NAMES Qt6 COMPONENTS Core Network Svg Widgets WebEngineWidgets REQUIRED)
find_package(Qt6 COMPONENTS Core Network Svg Widgets WebEngineWidgets REQUIRED)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6gui6, libqt6network6, libqt6svg6, libqt6widgets6, libqt6webenginewidgets6, libqt6webenginecore6")

include(CPack)

if (UNIX AND NOT APPLE)
    find_package(Qt6 COMPONENTS DBus REQUIRED)
endif()

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
add_subdirectory(src/3rdparty/singleapplication)

set(PROJECT_SOURCES
    src/debug.cpp
    src/edge.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/player.cpp
    src/powermanagement.cpp
    src/settings.cpp
    src/settingswidget.cpp
    src/settingswidget.ui
    src/serverdiscovery.cpp
    src/startup.cpp
    src/svgicon.cpp
    src/themes.cpp
    src/tmpwebenginepage.cpp
    src/webenginepage.cpp
    src/3rdparty/columnresizer/columnresizer.cpp
    melodeon.qrc
)

include_directories(src src/3rdparty/columnresizer ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
if (UNIX AND NOT APPLE)
    include_directories(src/linux)
    set(PROJECT_SOURCES ${PROJECT_SOURCES}
        src/linux/mpris.cpp
        src/linux/linuxpowermanagement.cpp
        )
    qt_add_dbus_adaptor(PROJECT_SOURCES src/linux/org.mpris.MediaPlayer2.Player.xml src/linux/mpris.h Mpris)
    qt_add_dbus_adaptor(PROJECT_SOURCES src/linux/org.mpris.MediaPlayer2.root.xml src/linux/mpris.h Mpris)
    qt_add_dbus_interface(PROJECT_SOURCES src/linux/org.freedesktop.login1.xml login1interface)
    qt_add_dbus_interface(PROJECT_SOURCES src/linux/org.freedesktop.PowerManagement.Inhibit.xml inhibitinterface)
    qt_add_dbus_interface(PROJECT_SOURCES src/linux/org.freedesktop.UPower.xml upowerinterface)
endif()

qt_add_executable(${EXECUTABLE_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Svg
    Qt6::Widgets
    Qt6::WebEngineWidgets
    SingleApplication::SingleApplication)

if (UNIX AND NOT APPLE)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE Qt6::DBus)
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${EXECUTABLE_NAME}.craigd.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(${EXECUTABLE_NAME})

set(SHARE_PREFIX ${CMAKE_INSTALL_PREFIX}/share/${EXECUTABLE_NAME})
configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

if (UNIX AND NOT APPLE)
    add_subdirectory(linux)
endif ()
