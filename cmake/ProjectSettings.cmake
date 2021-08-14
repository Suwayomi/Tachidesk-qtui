set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(IGNORE_WARNINGS "turn syntax warnings off" OFF)
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." OFF)
option(UsePrecompiledHeaders "Use precompiled headers" ON)
option(UninitializedMemberChecks "Use uninitialized member checking plugin" OFF)

# This generates csv files containing cross-file symbol dependencies
if(SymbolUseAnalyzer)
  if("${CMAKE_CXX_COMPILER}" MATCHES "/clang\\+\\+$")
    add_definitions(
      "-Xclang -load -Xclang ${CMAKE_CURRENT_BINARY_DIR}/clang/deptool/libsymbol-use-analyzer.so -Xclang -add-plugin -Xclang generate-symbol-tables"
    )
  endif("${CMAKE_CXX_COMPILER}" MATCHES "/clang\\+\\+$")
endif(SymbolUseAnalyzer)

# If using clang, add an option to enable the uninitialized member checks
if(UninitializedMemberChecks)
  if("${CMAKE_CXX_COMPILER}" MATCHES "/clang\\+\\+$")
    add_definitions(
      "-Xclang -load -Xclang ${CMAKE_CURRENT_BINARY_DIR}/clang/uninitialized-member/libuninitialized-member.so -Xclang -add-plugin -Xclang check-initializer-lists"
    )
  endif("${CMAKE_CXX_COMPILER}" MATCHES "/clang\\+\\+$")
endif(UninitializedMemberChecks)

# Option to enable pretty output when Ninja is used
if(${FORCE_COLORED_OUTPUT})
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

list(
  APPEND
  CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}"
  "${CMAKE_BINARY_DIR}"
)

# Encourage out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
  message(WARNING "You are building in-source.
      This can result in generated files being put in version control unintentionally.
      Please consider createing a build folder and building from there."
  )
endif()

# Set default build type
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE
  )
  # Set the possible values of build type for cmake-gui
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
  )
endif()

# Enable ccache if not using pch
if(NOT UsePrecompiledHeaders)
  find_program(CCACHE ccache)
  if(CCACHE)
    message("using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  else()
    message("ccache not found cannot use")
  endif()
endif()

include(ProjectFlags)
