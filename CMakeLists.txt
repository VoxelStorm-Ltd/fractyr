cmake_minimum_required(VERSION 3.20)

project(Fractyr LANGUAGES CXX)

if(WIN32)
  enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
    "Available build configurations" FORCE)
elseif(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Build type: Debug or Release" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(_build_32bit_default ON)
else()
  set(_build_32bit_default OFF)
endif()
option(BUILD_32BIT "Build the 32-bit target (-m32)" ${_build_32bit_default})

if(APPLE AND BUILD_32BIT)
  message(FATAL_ERROR "Fractyr only has a 64-bit macOS target")
endif()

if(BUILD_32BIT)
  set(FRACTYR_ARCH_FLAG -m32)
  set(FRACTYR_ARCH_BITS 32)
else()
  set(FRACTYR_ARCH_FLAG -m64)
  set(FRACTYR_ARCH_BITS 64)
endif()

if(WIN32)
  set(FRACTYR_PLATFORM Win)
elseif(APPLE)
  set(FRACTYR_PLATFORM Mac)
else()
  set(FRACTYR_PLATFORM Linux)
endif()

string(TOLOWER "${FRACTYR_PLATFORM}${FRACTYR_ARCH_BITS}"
  FRACTYR_PLATFORM_DIRECTORY)
set(FRACTYR_LIB_DIR
  "${CMAKE_SOURCE_DIR}/lib/${FRACTYR_PLATFORM_DIRECTORY}")

if(CMAKE_CROSSCOMPILING)
  find_path(FRACTYR_HOST_BOOST_INCLUDE_DIR
    NAMES boost/version.hpp
    HINTS ENV BOOST_ROOT ENV BOOST_INCLUDEDIR
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES include
    NO_CMAKE_FIND_ROOT_PATH
  )
  if(NOT FRACTYR_HOST_BOOST_INCLUDE_DIR)
    message(FATAL_ERROR "Boost headers could not be located on the build host")
  endif()

  get_filename_component(_boost_header_source
    "${FRACTYR_HOST_BOOST_INCLUDE_DIR}/boost" REALPATH)
  string(MD5 _boost_header_path_hash "${_boost_header_source}")
  set(_boost_isolated_root
    "${CMAKE_BINARY_DIR}/boost-include-${_boost_header_path_hash}")
  set(_boost_isolated_link "${_boost_isolated_root}/boost")
  file(MAKE_DIRECTORY "${_boost_isolated_root}")

  if(NOT EXISTS "${_boost_isolated_link}")
    file(CREATE_LINK
      "${_boost_header_source}"
      "${_boost_isolated_link}"
      SYMBOLIC
      RESULT _boost_link_result)
    if(NOT _boost_link_result STREQUAL "0")
      message(FATAL_ERROR
        "Could not create isolated Boost include link: ${_boost_link_result}")
    endif()
  endif()

  set(FRACTYR_BOOST_INCLUDE_DIRS "${_boost_isolated_root}")
else()
  if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
  endif()
  find_package(Boost QUIET CONFIG)
  if(NOT Boost_FOUND)
    find_package(Boost REQUIRED MODULE)
  endif()
  set(FRACTYR_BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
endif()

find_program(BASH_EXECUTABLE bash REQUIRED)

set(FRACTYR_GIT_VERSION_HEADER "${CMAKE_SOURCE_DIR}/git_version.h")
set(_git_log_head "${CMAKE_SOURCE_DIR}/.git/logs/HEAD")
if(EXISTS "${_git_log_head}")
  set(_git_version_extra_depends DEPENDS "${_git_log_head}")
else()
  set(_git_version_extra_depends "")
endif()

add_custom_command(
  OUTPUT "${FRACTYR_GIT_VERSION_HEADER}"
  COMMAND ${BASH_EXECUTABLE} resources/generate_git_version.sh
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/resources/generate_git_version.sh"
  ${_git_version_extra_depends}
  COMMENT "Generating git_version.h"
  VERBATIM
)
add_custom_target(fractyr_git_version
  DEPENDS "${FRACTYR_GIT_VERSION_HEADER}"
)

set(FRACTYR_BLOB_RESOURCES
  resources/music_main_intro.ogg
  resources/music_main_loop.ogg
  resources/shaders/chunk_frag.glsl
  resources/shaders/chunk_vert.glsl
  resources/shaders/enemy_grunt_frag.glsl
  resources/shaders/enemy_grunt_vert.glsl
  resources/shaders/plasma_frag.glsl
  resources/shaders/plasma_vert.glsl
  resources/title.ttf
)

set(FRACTYR_BLOB_OBJECT_DIRECTORY "${CMAKE_BINARY_DIR}/blob_objects")
file(MAKE_DIRECTORY "${FRACTYR_BLOB_OBJECT_DIRECTORY}")
set(FRACTYR_BLOB_OBJECTS "")

foreach(_blob IN LISTS FRACTYR_BLOB_RESOURCES)
  string(REPLACE "/" "_" _blob_object_name "${_blob}")
  string(REPLACE "." "_" _blob_object_name "${_blob_object_name}")
  set(_blob_object
    "${FRACTYR_BLOB_OBJECT_DIRECTORY}/${_blob_object_name}.o")
  add_custom_command(
    OUTPUT "${_blob_object}"
    COMMAND ${BASH_EXECUTABLE}
      "${CMAKE_SOURCE_DIR}/resources/compile_blob.sh"
      "${_blob}"
      "${_blob_object}"
      "${CMAKE_CXX_COMPILER}"
      "${FRACTYR_ARCH_FLAG}"
      ""
    DEPENDS
      "${CMAKE_SOURCE_DIR}/${_blob}"
      "${CMAKE_SOURCE_DIR}/resources/compile_blob.sh"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Compiling blob resource ${_blob}"
    VERBATIM
  )
  list(APPEND FRACTYR_BLOB_OBJECTS "${_blob_object}")
endforeach()

add_custom_target(fractyr_blob_resources
  DEPENDS ${FRACTYR_BLOB_OBJECTS}
)

set(FRACTYR_SOURCES
  buffers/buffer.cpp
  buffers/buffer_chunk.cpp
  buffers/buffer_enemy_core.cpp
  buffers/buffer_enemy_grunt.cpp
  buffers/buffer_plasma.cpp
  buffers/buffer_shard.cpp
  callback_error.cpp
  chunk.cpp
  entity/bullet/bullet.cpp
  entity/bullet/plasma.cpp
  entity/enemy/core.cpp
  entity/enemy/enemy.cpp
  entity/enemy/grunt.cpp
  entity/entity.cpp
  entity/particle/particle.cpp
  entity/playership.cpp
  entity/ship.cpp
  font_load.cpp
  gameplayer.cpp
  get_version.cpp
  init_graphics.cpp
  input/callback_key.cpp
  input/callback_mouse.cpp
  input/callback_mousebutton.cpp
  input/callback_scroll.cpp
  input/callback_textinput.cpp
  input/callback_windowclose.cpp
  input/callback_windowfocus.cpp
  input/callback_windowminimise.cpp
  input/callback_windowrefresh.cpp
  input/callback_windowresize.cpp
  input/controls.cpp
  main.cpp
  oculusstorm/oculusstorm.cpp
  shader_load.cpp
  soundstorm/soundstorm.cpp
  universe.cpp
  vectorstorm/vectorstorm.cpp
  weapon/blaster.cpp
  weapon/gruntblaster.cpp
  weapon/weapon.cpp
  world.cpp
)

if(WIN32)
  list(APPEND FRACTYR_SOURCES fractyr.rc)
endif()

add_executable(Fractyr ${FRACTYR_SOURCES})

set_target_properties(Fractyr PROPERTIES
  WIN32_EXECUTABLE "$<CONFIG:Release>"
)

set_source_files_properties(${FRACTYR_BLOB_OBJECTS} PROPERTIES
  EXTERNAL_OBJECT TRUE
  GENERATED TRUE
)
target_sources(Fractyr PRIVATE ${FRACTYR_BLOB_OBJECTS})
add_dependencies(Fractyr fractyr_blob_resources fractyr_git_version)

target_include_directories(Fractyr PRIVATE
  "${CMAKE_SOURCE_DIR}"
)
target_include_directories(Fractyr SYSTEM PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
  ${FRACTYR_BOOST_INCLUDE_DIRS}
)

target_compile_definitions(Fractyr PRIVATE
  BOOST_THREAD_VERSION=3
  BOOST_THREAD_USE_LIB
  BOOST_SYSTEM_NO_DEPRECATED
  GLEW_STATIC
  $<$<CONFIG:Debug>:LOGGING>
  $<$<CONFIG:Debug>:IGNORE_DEBUG_INPUTSTORM>
  $<$<CONFIG:Debug>:IGNORE_DEBUG_SOUNDSTORM>
  $<$<CONFIG:Release>:NDEBUG>
  $<$<CONFIG:Release>:BOOST_DISABLE_ASSERTS>
)

if(WIN32 AND CMAKE_HOST_UNIX)
  target_compile_definitions(Fractyr PRIVATE BLOB_LOADER_LINK_MODE=2)
endif()

target_compile_options(Fractyr PRIVATE
  ${FRACTYR_ARCH_FLAG}
  -mmmx
  -msse
  -msse2
  -m3dnow
  -Wnon-virtual-dtor
  -Wshadow
  -Winit-self
  -Wredundant-decls
  -Wcast-align
  -Winline
  -Wmissing-declarations
  -Wmissing-include-dirs
  -Wfatal-errors
  -Wextra
  -Wall
  -Wunused
  -Wuninitialized
  -Wmaybe-uninitialized
  -Wconversion
  -Wuseless-cast
  -Warray-bounds
  -Woverloaded-virtual
  -Wdouble-promotion
  -Wsuggest-attribute=pure
  -Wsuggest-attribute=const
  -Wsuggest-attribute=noreturn
  -Wsuggest-final-types
  -Wsuggest-final-methods
  -Wsuggest-override
  -Wstack-usage=2097152
  -Wunsafe-loop-optimizations
  -Wpointer-arith
  -Wlogical-op
  -Wnormalized
  -Wpacked
  -Winvalid-pch
  -Wlong-long
  -Wvector-operation-performance
  -Wdisabled-optimization
  $<$<CONFIG:Debug>:-Og>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-ggdb3>
)

if(WIN32)
  target_compile_options(Fractyr PRIVATE -mthreads)
elseif(APPLE)
  target_compile_options(Fractyr PRIVATE -mmacosx-version-min=10.12)
endif()

if(UNIX AND NOT APPLE AND NOT BUILD_32BIT)
  target_compile_options(Fractyr PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=undefined>
    $<$<CONFIG:Debug>:-fsanitize=float-divide-by-zero>
    $<$<CONFIG:Debug>:-fsanitize=float-cast-overflow>
    $<$<CONFIG:Debug>:-fsanitize=address>
    $<$<CONFIG:Debug>:-fno-sanitize-recover>
    $<$<CONFIG:Debug>:-fstack-protector>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    $<$<CONFIG:Debug>:-fno-optimize-sibling-calls>
  )
endif()

set(FRACTYR_RELEASE_OPTIONS
  -Ofast
  -fexpensive-optimizations
  -flto
  -maccumulate-outgoing-args
  -fgcse-sm
  -fgcse-las
  -fgcse-after-reload
  -fsched-spec-load
  -fsched-spec-load-dangerous
  -fmerge-all-constants
  -fmodulo-sched
  -fmodulo-sched-allow-regmoves
  -funsafe-loop-optimizations
  -fsched-pressure
  -fsched-stalled-insns=0
  -fsched-stalled-insns-dep=32
  -fsched2-use-superblocks
  -fipa-pta
  -ftracer
  -fno-var-tracking-assignments
  -fpeel-loops
  -funroll-loops
  -funswitch-loops
  -ftree-loop-if-convert
  -ftree-loop-distribution
  -ftree-loop-im
  -ftree-loop-ivcanon
  -fivopts
  -fvariable-expansion-in-unroller
  -freorder-blocks-and-partition
  -fbranch-target-load-optimize
  -ffast-math
  -fno-math-errno
  -funsafe-math-optimizations
  -fassociative-math
  -freciprocal-math
  -fno-signed-zeros
  -fno-trapping-math
  --param=max-gcse-memory=1000000000
  --param=max-inline-insns-single=10000
  --param=max-inline-insns-auto=10000
  --param=inline-min-speedup=20
  --param=large-function-insns=10000
  --param=large-function-growth=2000
  --param=large-unit-insns=20000
  --param=inline-unit-growth=200
)
if(NOT APPLE)
  list(APPEND FRACTYR_RELEASE_OPTIONS -fuse-linker-plugin)
endif()
target_compile_options(Fractyr PRIVATE
  "$<$<CONFIG:Release>:${FRACTYR_RELEASE_OPTIONS}>"
)
if(WIN32 AND BUILD_32BIT)
  target_compile_options(Fractyr PRIVATE
    $<$<CONFIG:Release>:-fomit-frame-pointer>
  )
endif()

target_link_options(Fractyr PRIVATE
  ${FRACTYR_ARCH_FLAG}
  -mmmx
  -msse
  -msse2
  -m3dnow
  $<$<CONFIG:Debug>:-Og>
  $<$<CONFIG:Debug>:-ggdb3>
  $<$<CONFIG:Release>:-s>
  "$<$<CONFIG:Release>:${FRACTYR_RELEASE_OPTIONS}>"
)

if(WIN32)
  target_link_options(Fractyr PRIVATE
    -static
    -static-libstdc++
    -static-libgcc
    -no-pie
    -Wl,--stack,8388608
    -mthreads
  )
elseif(APPLE)
  target_link_options(Fractyr PRIVATE
    -static-libstdc++
    -fno-pie
    -mmacosx-version-min=10.12
    -framework OpenGL
    -framework IOKit
    -framework AppKit
    -framework CoreFoundation
    -framework ApplicationServices
    -framework CoreAudio
    -framework AudioToolbox
    -framework AudioUnit
    -framework QuartzCore
    -framework Carbon
  )
else()
  target_link_options(Fractyr PRIVATE
    -static-libstdc++
    -static-libgcc
    -no-pie
    -pthread
  )
  if(BUILD_32BIT)
    target_link_options(Fractyr PRIVATE -static)
  else()
    target_link_options(Fractyr PRIVATE
      $<$<CONFIG:Debug>:-fuse-ld=gold>
      $<$<CONFIG:Debug>:-Wl,--threads>
      $<$<CONFIG:Debug>:-Wl,--thread-count=60>
    )
  endif()
endif()

if(WIN32 AND BUILD_32BIT)
  target_link_libraries(Fractyr PRIVATE
    "${FRACTYR_LIB_DIR}/libvoro++.a"
    "${FRACTYR_LIB_DIR}/libglew32.a"
    "${FRACTYR_LIB_DIR}/libglfw3.a"
    "$<IF:$<CONFIG:Debug>,${FRACTYR_LIB_DIR}/libftgl_static_D.a,${FRACTYR_LIB_DIR}/libftgl_static.a>"
    "${FRACTYR_LIB_DIR}/libglu32.a"
    "${FRACTYR_LIB_DIR}/libopengl32.a"
    "${FRACTYR_LIB_DIR}/libfreetype.a"
    "$<IF:$<CONFIG:Debug>,${FRACTYR_LIB_DIR}/libOVR_MinGW_Win32_d.a,${FRACTYR_LIB_DIR}/libOVR_MinGW_Win32.a>"
    "${FRACTYR_LIB_DIR}/libportaudiocpp_static.a"
    "${FRACTYR_LIB_DIR}/libportaudio_static_x86.a"
    "${FRACTYR_LIB_DIR}/libvorbisfile.a"
    "${FRACTYR_LIB_DIR}/libvorbis.a"
    "${FRACTYR_LIB_DIR}/libogg.a"
    "${FRACTYR_LIB_DIR}/libws2_32.a"
    "${FRACTYR_LIB_DIR}/libgdi32.a"
    "${FRACTYR_LIB_DIR}/libwinmm.a"
    "${FRACTYR_LIB_DIR}/libole32.a"
    "${FRACTYR_LIB_DIR}/libsetupapi.a"
    "${FRACTYR_LIB_DIR}/libz.a"
  )
elseif(APPLE)
  target_link_libraries(Fractyr PRIVATE
    "${FRACTYR_LIB_DIR}/libvoro++.a"
    "$<IF:$<CONFIG:Debug>,${FRACTYR_LIB_DIR}/libOVR_MinGW_Mac64_d.a,${FRACTYR_LIB_DIR}/libOVR_MinGW_Mac64.a>"
    "${FRACTYR_LIB_DIR}/libportaudio.a"
    "${FRACTYR_LIB_DIR}/libportaudiocpp.a"
    "${FRACTYR_LIB_DIR}/libftgl.a"
    "${FRACTYR_LIB_DIR}/libvorbis.a"
    "${FRACTYR_LIB_DIR}/libvorbisfile.a"
    "${FRACTYR_LIB_DIR}/libogg.a"
    "${FRACTYR_LIB_DIR}/libglfw3.a"
    "${FRACTYR_LIB_DIR}/libfreetype.a"
    "${FRACTYR_LIB_DIR}/libGLEW.a"
    "${FRACTYR_LIB_DIR}/libbz2.a"
    "${FRACTYR_LIB_DIR}/libpng16.a"
    "${FRACTYR_LIB_DIR}/libz.a"
  )
elseif(UNIX AND NOT BUILD_32BIT)
  target_link_libraries(Fractyr PRIVATE
    $<$<CONFIG:Debug>:asan>
    $<$<CONFIG:Debug>:ubsan>
    "${FRACTYR_LIB_DIR}/libvoro++.a"
    "${FRACTYR_LIB_DIR}/libGLEW.a"
    "${FRACTYR_LIB_DIR}/libglfw3.a"
    Xrandr
    "${FRACTYR_LIB_DIR}/libftgl.a"
    "${FRACTYR_LIB_DIR}/libGLU.a"
    GL
    "${FRACTYR_LIB_DIR}/libfreetype.a"
    "$<IF:$<CONFIG:Debug>,${FRACTYR_LIB_DIR}/libOVR_MinGW_Linux64_d.a,${FRACTYR_LIB_DIR}/libOVR_MinGW_Linux64.a>"
    "${FRACTYR_LIB_DIR}/libportaudio.a"
    "${FRACTYR_LIB_DIR}/libportaudiocpp.a"
    asound
    "${FRACTYR_LIB_DIR}/libvorbisfile.a"
    "${FRACTYR_LIB_DIR}/libvorbis.a"
    "${FRACTYR_LIB_DIR}/libogg.a"
    dl
    Xt
    Xi
    X11
    Xxf86vm
    udev
    "${FRACTYR_LIB_DIR}/libXinerama.a"
    "${FRACTYR_LIB_DIR}/libpng16.a"
    "${FRACTYR_LIB_DIR}/libz.a"
    "${FRACTYR_LIB_DIR}/libXcursor.a"
    "${FRACTYR_LIB_DIR}/libXfixes.a"
    "${FRACTYR_LIB_DIR}/libXrender.a"
    "${FRACTYR_LIB_DIR}/libXext.a"
  )
elseif(UNIX)
  target_link_libraries(Fractyr PRIVATE
    "${FRACTYR_LIB_DIR}/libGLEW.a"
    "${FRACTYR_LIB_DIR}/libglfw3.a"
    Xrandr
    "${FRACTYR_LIB_DIR}/libftgl.a"
    "${FRACTYR_LIB_DIR}/libGLU.a"
    GL
    "${FRACTYR_LIB_DIR}/libfreetype.a"
    Xt
    Xm
    pthread
    udev
    Xinerama
  )
endif()
