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

if (ANDROID)
    include(${CMAKE_SOURCE_DIR}/../android_openssl/CMakeLists.txt)
endif()

function(include_directories_recurse dir)
	include_directories(${dir})
	file(GLOB subs "${dir}/*")
	foreach(sub ${subs})
		if (IS_DIRECTORY ${sub})
			include_directories_recurse(${sub})
		endif (IS_DIRECTORY ${sub})
	endforeach(sub ${subs})
endfunction(include_directories_recurse dir)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Locate all the source and header files for svs
file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE src_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.h")

list(APPEND src_files ${add_src_files})

include_directories_recurse(src)

set(lib_srcs ${src_files})

if (ANDROID)
  set(ANDROID_PACKAGE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/android CACHE INTERNAL "")
  add_library(qtui SHARED ${lib_srcs} src/qml/qml.qrc android/AndroidManifest.xml android/res/values/styles.xml)
else()
  qt_add_executable(qtui MACOSX_BUNDLE ${lib_srcs} src/qml/qml.qrc)

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

target_include_directories(qtui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
