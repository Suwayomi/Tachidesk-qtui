cmake_minimum_required(VERSION 3.14)

project(tachidesk-qtui)

include(cmake/ProjectSettings.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(
  Qt6
  COMPONENTS Core
             Gui
             Quick
             Network
             Widgets
             WebSockets
  REQUIRED)

add_subdirectory(libs/SortFilterProxyModel)

set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
# And the following tells CMake where to find and install the file itself.
set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/images/AppIcon.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

qt_add_executable(tachidesk-qtui MACOSX_BUNDLE ${app_icon_macos} MANUAL_FINALIZATION)

if (ANDROID)
    include(${CMAKE_SOURCE_DIR}/../android_openssl/CMakeLists.txt)
endif()

# Locate all the source and header files
file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE src_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.h")
file(GLOB_RECURSE qml_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.qml")
file(GLOB_RECURSE icon_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "sources/images/*.png" "android/res/*.png")

list(APPEND src_files ${add_src_files})

qt_add_qml_module(tachidesk-qtui
  URI Tachidesk.Qtui
  VERSION 1.0
  SOURCES
    ${src_files}
    ${header_files}
  QML_FILES
    ${qml_files}
  RESOURCES
    ${icon_files}
    libs/QmlBridgeForMaterialDesignIcons/materialdesignicons-webfont.ttf
    libs/QmlBridgeForMaterialDesignIcons/Icon.js
    android/AndroidManifest.xml
    android/res/values/styles.xml
  IMPORTS
    QtQml
    QtQuick
    QtQuick.Controls
    QtQuick.Layouts
    SortFilterProxyModel)

target_include_directories(tachidesk-qtui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/models>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utils>
    $<INSTALL_INTERFACE:include>)

if (ANDROID)
  set_target_properties(tachidesk-qtui PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)

  set_property(TARGET tachidesk-qtui PROPERTY
    QT_ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS})

  #set(ANDROID_PACKAGE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/android CACHE INTERNAL "")

  #add_library(qtui SHARED ${lib_srcs} android/AndroidManifest.xml android/res/values/styles.xml)
else()

  if (APPLE)
     set_target_properties(tachidesk-qtui PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.suwayomi.tachidesk-qtui"
        MACOSX_BUNDLE_BUNDLE_VERSION 1
        MACOSX_BUNDLE_SHORT_VERSION_STRING  1.0)

     SET_SOURCE_FILES_PROPERTIES(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION ios)
     SET_PROPERTY(TARGET tachidesk-qtui PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist)
  endif()

endif()

target_link_libraries(tachidesk-qtui
  PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::WebSockets
    SortFilterProxyModel)

qt_finalize_executable(tachidesk-qtui)

if (NOT ANDROID AND NOT APPLE)
  install (TARGETS tachidesk-qtui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME Tachidesk-QtUI)
endif()

if(UNIX AND NOT APPLE)
  set(APPID "tachidesk-qtui")

	install (TARGETS tachidesk-qtui RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
	install (FILES "resources/tachidesk-qtui-16.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-32.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-48.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-64.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-128.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-256.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui-512.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps" RENAME "${APPID}.png")
	install (FILES "resources/tachidesk-qtui.svg" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps" RENAME "${APPID}.svg")

	configure_file("resources/tachidesk-qtui.desktop.in" "resources/tachidesk-qtui.desktop" @ONLY)
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/resources/tachidesk-qtui.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications" RENAME "${APPID}.desktop")

endif()
