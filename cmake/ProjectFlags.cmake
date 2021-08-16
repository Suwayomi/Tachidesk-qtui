if(MSVC)
  add_compile_options(/W4 /bigobj /permissive-)
  add_compile_definitions(
    _SCL_SECURE_NO_WARNINGS
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _ENABLE_ATOMIC_ALIGNMENT_FIX
    _LIB
    NOMINMAX
    $<$<CONFIG:Debug>:_DEBUG>
    WIN32_CMAKE
  )
  if(NOT
     CMAKE_CXX_COMPILER_ID
     STREQUAL
     "Clang"
  )
    add_compile_options(
      /experimental:external
      /external:anglebrackets
      /external:templates-
      /external:W0
    )
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
  add_compile_options("$<$<CONFIG:Debug>:-ggdb3;-O0>")

  # These GNU extensions are supported by all of the compilers we use
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options(-Wno-nested-anon-types)
    add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
    add_compile_options(-Wno-c99-extensions)
    add_compile_options(-Wno-format-security)
  endif()

  add_compile_definitions(_GLIBCXX_USE_SCHED_YIELD _GLIBCXX_USE_NANOSLEEP)

  add_compile_options($<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-ftemplate-depth-1024>)
endif()

# Until protobuf gets it together, ignore deprecation warnings
if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
  add_compile_options(-Wno-deprecated-declarations)
endif ()

if(CMAKE_CROSSCOMPILING)
 add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
 if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
 endif()
endif()

if(APPLE)
 set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG 1)
endif()
