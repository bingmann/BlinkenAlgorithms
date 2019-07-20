################################################################################
# blinken-sort-sound-pi/CMakeLists.txt
#
# Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
#
# All rights reserved. Published under the GNU General Public License v3.0
################################################################################

cmake_minimum_required(VERSION 2.8)

project(blinken-sort)

# prohibit in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif()

# default to Debug building for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Defaulting CMAKE_BUILD_TYPE to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# enable warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdelete-non-virtual-dtor")
set(CMAKE_CXX_STANDARD "14")

if(NOT WIN32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  # remove -rdynamic from linker flags (smaller binaries which cannot be loaded
  # with dlopen() -- something no one needs)
  string(REGEX REPLACE "-rdynamic" ""
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
  string(REGEX REPLACE "-rdynamic" ""
    CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")
endif()

# enable use of "make test"
enable_testing()

# enable -march=native on Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT MINGW)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-march=native THRILL_HAS_MARCH_NATIVE)
  if(THRILL_HAS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
  endif()
endif()

################################################################################
### Find Required Libraries ###

### use pthread ###

find_package(Threads)

### use SDL and SDL_sound ###

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

################################################################################
### Compile Programs

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/BlinkenAlgorithms)

add_executable(blinken-sort
  blinken-sort.cpp
  )

target_link_libraries(blinken-sort
  ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
  )

################################################################################
