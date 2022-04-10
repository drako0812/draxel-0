cmake_minimum_required(VERSION 3.21)

project(
    Pixelly2
    VERSION 0.1.0
    LANGUAGES CXX C
)

###############################################################################
#
# SETUP VARIABLES
#
###############################################################################

if(NOT DEFINED VCPKG_ROOT)
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT $ENV{VCPKG_ROOT} CACHE PATH "Path to vcpkg directory")
    endif()
endif()

if(DEFINED VCPKG_ROOT)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    
    if(NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        message(FATAL_ERROR "Invalid VCPKG_ROOT specified (${VCPKG_ROOT}); please specify an absolute path")
    endif()
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOL2_DO_TESTS OFF)
set(SOL2_DO_EXAMPLES OFF)
set(SOL2_DOCS OFF)
set(SOL2_LUA_VERSION "5.4")
set(SOL2_BUILD_LUA OFF)

add_compile_definitions(
    SOL_ALL_SAFETIES_ON
    SOL_USING_CXX_LUA
)

###############################################################################
#
# FIND PACKAGES
#
###############################################################################

find_package(effolkronium_random CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(teemo CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_path(BOOST_INCLUDE_DIR "boost/predef.h")

set(SFML_BUILD_WINDOW ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_NETWORK OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)
set(SFML_ENABLE_COVERAGE OFF)
add_subdirectory(deps/SFML)

#add_compile_definitions(_SQ64)
#add_subdirectory(deps/quirrel)

###############################################################################
#
# SET UP TARGETS
#
###############################################################################

add_executable(Pixelly
    src/Pixelly2.cpp
    src/base.cpp
    src/gpu.cpp
    src/input.cpp
    src/machine.cpp
    src/apis/lua.cpp
)
target_include_directories(Pixelly
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${LUA_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIRS}
)
target_compile_features(Pixelly PUBLIC cxx_std_20)
target_compile_features(Pixelly PUBLIC c_std_17)
target_link_libraries(Pixelly
    PRIVATE
        sfml-main
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        effolkronium_random
        fmt::fmt
        ${LUA_LIBRARIES}
        sol2::sol2
        teemo
        ZLIB::ZLIB
)
if(CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(Pixelly PUBLIC PXLY_DEBUG)
endif()

set(PXLY_DATA_FILES
    data/fonts/unscii-8-alt-mod.otf
    data/fonts/unscii-8-fantasy-mod.otf
    data/fonts/unscii-8-mcr-mod.otf
    data/fonts/unscii-8-mod.otf
    data/fonts/unscii-8-thin-mod.otf
    data/scripts/lua/boot.lua
)

foreach(PXLY_DATA_FILE ${PXLY_DATA_FILES})
    add_custom_command(TARGET Pixelly POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${PXLY_DATA_FILE}
            ${CMAKE_BINARY_DIR}/${PXLY_DATA_FILE}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${PXLY_DATA_FILE}
    )
endforeach()
