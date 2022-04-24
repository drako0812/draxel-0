cmake_minimum_required(VERSION 3.21)

project(
    draxel-0
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
find_path(BOOST_INCLUDE_DIR "boost/predef.h")

set(SFML_BUILD_WINDOW ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_NETWORK OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TEST_SUITE OFF)
set(SFML_ENABLE_COVERAGE OFF)
add_subdirectory(deps/SFML)

add_subdirectory(deps/gcem)

#add_compile_definitions(_SQ64)
#add_subdirectory(deps/quirrel)

###############################################################################
#
# SET UP TARGETS
#
###############################################################################

add_executable(draxel-0
    src/draxel-0.cpp
    src/base.cpp
    src/gpu.cpp
    src/gpu_textmode.cpp
    src/gpu_palette.cpp
    src/gpu_pixmode1.cpp
    src/gpu_texture1.cpp
    src/gpu_textures1.cpp
    src/input.cpp
    src/machine.cpp
    src/apis/lua.cpp
    src/apis/lua/lua_gpu.cpp
    src/apis/lua/lua_input.cpp
    src/apis/lua/lua_intern.cpp
)
target_include_directories(draxel-0
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/internal
        ${LUA_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIRS}
)
target_compile_features(draxel-0 PUBLIC cxx_std_20)
target_compile_features(draxel-0 PUBLIC c_std_17)

if(UNIX AND NOT APPLE)
    set(DRXL_SFML_LIBRARIES
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
    )
else()
    set(DRXL_SFML_LIBRARIES
        sfml-main
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
    )
endif()

if(WIN32)
    target_compile_options(draxel-0 PRIVATE "/bigobj" "/FAsu" "/Fa")
endif()

target_link_libraries(draxel-0
    PRIVATE
        ${DRXL_SFML_LIBRARIES}
        effolkronium_random
        fmt::fmt
        ${LUA_LIBRARIES}
        sol2::sol2
        gcem
)
if(CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(draxel-0 PUBLIC DRXL_DEBUG)
endif()

set(DRXL_DATA_FILES
    data/fonts/unscii-8-alt-mod.otf
    data/fonts/unscii-8-fantasy-mod.otf
    data/fonts/unscii-8-mcr-mod.otf
    data/fonts/unscii-8-mod.otf
    data/fonts/unscii-8-thin-mod.otf
    data/scripts/lua/boot.lua
)

foreach(DRXL_DATA_FILE ${DRXL_DATA_FILES})
    add_custom_command(TARGET draxel-0 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${DRXL_DATA_FILE}
            ${CMAKE_BINARY_DIR}/${DRXL_DATA_FILE}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${DRXL_DATA_FILE}
    )
endforeach()
