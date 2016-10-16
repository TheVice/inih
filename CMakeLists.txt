
cmake_minimum_required(VERSION 2.8.12)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Build process cannot start from project source directory")
endif()

project("inih")

set(PROJECT_DEFINITIONS)

if(MSVC)
  if(NOT CHARACTER_SET)
    list(APPEND PROJECT_DEFINITIONS _UNICODE UNICODE)
  else()
    list(APPEND PROJECT_DEFINITIONS _MBCS)
  endif()
  list(APPEND PROJECT_DEFINITIONS NOMINMAX)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND PROJECT_DEFINITIONS __clang__)
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  list(APPEND PROJECT_DEFINITIONS _GNU_SOURCE)
endif()

if(WIN32)
  list(APPEND PROJECT_DEFINITIONS WIN32)
  list(APPEND PROJECT_DEFINITIONS _WIN32)
  list(APPEND PROJECT_DEFINITIONS _WINDOWS)
  list(APPEND PROJECT_DEFINITIONS NOCOMM)
  list(APPEND PROJECT_DEFINITIONS WIN32_LEAN_AND_MEAN)
  list(APPEND PROJECT_DEFINITIONS _WIN32_WINNT=0x0500)
endif()

# ini
set(Ini_Path ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ini "${Ini_Path}/ini.c" "${Ini_Path}/ini.h")
if(PROJECT_DEFINITIONS)
  set_target_properties(ini PROPERTIES COMPILE_DEFINITIONS "${PROJECT_DEFINITIONS}")
endif()

# INIReader
set(Ini_Reader_Path ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
add_library(ini_reader "${Ini_Reader_Path}/INIReader.cpp" "${Ini_Reader_Path}/INIReader.h")
if(PROJECT_DEFINITIONS)
  set_target_properties(ini_reader PROPERTIES COMPILE_DEFINITIONS "${PROJECT_DEFINITIONS}")
endif()

# Copy assets
add_custom_target(copyAssets)

set(sourcePath ${CMAKE_CURRENT_SOURCE_DIR}/examples/test.ini)
set(destinationPath ${CMAKE_BINARY_DIR}/examples/test.ini)

add_custom_command(TARGET copyAssets PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${sourcePath}" "${destinationPath}")

file(GLOB inis ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.ini)
foreach(sourcePath ${inis})
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${CMAKE_BINARY_DIR}" destinationPath ${sourcePath})

  add_custom_command(TARGET copyAssets PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${sourcePath}" "${destinationPath}")
endforeach()

# examples
file(GLOB examples RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.c)
foreach(example ${examples})
  string(FIND ${example} ".c" pos0 REVERSE)
  string(SUBSTRING ${example} 0 ${pos0} example_name)
  add_executable(${example_name} "${CMAKE_CURRENT_SOURCE_DIR}/examples/${example}")
  target_link_libraries(${example_name} ini)
  if(PROJECT_DEFINITIONS)
    set_target_properties(${example_name} PROPERTIES COMPILE_DEFINITIONS "${PROJECT_DEFINITIONS}")
  endif()
endforeach()

# tests
set(Tests_Path ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_executable(tests "${Tests_Path}/unittest.c")
target_link_libraries(tests ini)
if(PROJECT_DEFINITIONS)
  set_target_properties(tests PROPERTIES COMPILE_DEFINITIONS "${PROJECT_DEFINITIONS}")
endif()
add_dependencies(tests copyAssets)

# Flags
if(MSVC)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(FLAGS "${FLAGS} -Wall -Wextra -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEFAULT_CMAKE_C_FLAGS} ${FLAGS} -std=c11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_CMAKE_CXX_FLAGS} ${FLAGS} -std=c++1y")
  else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /W4")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc /W0 /GS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /W0 /GS")
    if(CMAKE_CL_64)
      set(LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} /DynamicBase /NXCompat")
    else()
      set(LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} /SafeSEH /DynamicBase /NXCompat")
    endif()
  endif()
else()
  set(FLAGS "${FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEFAULT_CMAKE_C_FLAGS} ${FLAGS} -std=c11")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_CMAKE_CXX_FLAGS} ${FLAGS} -std=c++1y")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_CMAKE_CXX_FLAGS} ${FLAGS} -std=c++14")
  endif()
endif()
