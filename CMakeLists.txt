cmake_minimum_required(VERSION 3.14)

project(Tachidesk-qtui)

include(cmake/ProjectSettings.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(
  Qt5
  COMPONENTS Core
             Gui
             Quick
             Multimedia
             Network
             Positioning
             Widgets
             Test
             Location
             QmlImportScanner
  REQUIRED)
find_package(Qt5QuickCompiler)

add_executable(Tachidesk-qtui src/qml/qml.qrc)

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
target_sources(
  Tachidesk-qtui
  PRIVATE ${lib_srcs})

target_link_libraries(Tachidesk-qtui
  Qt::Quick
  Qt::Core
  Qt::Widgets
  Qt::Gui)

target_include_directories(Tachidesk-qtui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

