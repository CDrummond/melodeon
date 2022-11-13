cmake_minimum_required(VERSION 3.5)

project(lms-material-app-qt VERSION 0.0.1 LANGUAGES CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "DEB;RPM;TBZ2")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MaterialSkin WebApp wrapper")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_CONTACT "Craig Drummond <craig.p.drummond@gmail.com>")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES  "/.svn/;/.git/;.gitignore;.github/;.project;CMakeLists.txt.user;/screenshots/")
include(CPack)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Widgets WebEngineWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Widgets WebEngineWidgets REQUIRED)

set(PROJECT_SOURCES
    src/columnresizer.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/settingswidget.cpp
    src/settingswidget.ui
    src/serverdiscovery.cpp
    src/startup.cpp
    src/themes.cpp
    src/webenginepage.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${EXECUTABLE_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(${EXECUTABLE_NAME} ${PROJECT_SOURCES})
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${EXECUTABLE_NAME}.craigd.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${EXECUTABLE_NAME})
endif()

set(SHARE_PREFIX ${CMAKE_INSTALL_PREFIX}/share/${EXECUTABLE_NAME})
configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

if (UNIX AND NOT APPLE)
    add_subdirectory(linux)
endif ()
