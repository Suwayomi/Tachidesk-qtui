cmake_minimum_required(VERSION 3.14)

project(qtui)

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

qt_add_executable(qtui MACOSX_BUNDLE MANUAL_FINALIZATION)

if (ANDROID)
    include(${CMAKE_SOURCE_DIR}/../android_openssl/CMakeLists.txt)

endif()

# Locate all the source and header files for svs
file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE src_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.h")
file(GLOB_RECURSE qml_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.qml")
file(GLOB_RECURSE icon_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "images/*.png" "android/res/*.png")

list(APPEND src_files ${add_src_files})

qt_add_qml_module(qtui
  URI Tachidesk.qtui
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

target_include_directories(qtui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/models>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/utils>)

if (ANDROID)
  set_target_properties(qtui PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)

  set_property(TARGET qtui PROPERTY
    QT_ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS})

  #set(ANDROID_PACKAGE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/android CACHE INTERNAL "")

  #add_library(qtui SHARED ${lib_srcs} android/AndroidManifest.xml android/res/values/styles.xml)
else()

  if (APPLE)
     set_target_properties(qtui PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.suwayomi.tachidesk-qtui"
        MACOSX_BUNDLE_BUNDLE_VERSION 1
        MACOSX_BUNDLE_SHORT_VERSION_STRING  1.0)

     SET_SOURCE_FILES_PROPERTIES(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
     SET_PROPERTY(TARGET qtui PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist)
  endif()

endif()

target_link_libraries(qtui
  PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::WebSockets
    SortFilterProxyModel)

qt_finalize_executable(qtui)

install (TARGETS qtui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME Tachidesk-QtUI)
