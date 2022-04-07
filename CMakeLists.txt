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

###############################################################################
#
# INCLUDE SUBDIRECTORIES
#
###############################################################################
set(WANT_OPENGL ON)
set(WANT_SHADERS_GL ON)
set(WANT_FONT ON)
set(WANT_AUDIO ON)
set(WANT_IMAGE ON)
set(WANT_TTF ON)
set(WANT_COLOR ON)
set(WANT_PRIMITIVES ON)
set(WANT_NATIVE_DIALOG ON)
set(WANT_VIDEO ON)
set(WANT_MONOLITH ON)
set(WANT_DOCS OFF)
add_subdirectory(deps/allegro5)

###############################################################################
#
# FIND PACKAGES
#
###############################################################################

#vcpkg_install_packages()
#find_package(allegro5 REQUIRED)
find_package(Threads REQUIRED)
find_package(Angelscript REQUIRED)
find_package(effolkronium_random REQUIRED)
find_package(fmt REQUIRED)

###############################################################################
#
# SET UP TARGETS
#
###############################################################################

add_subdirectory(PixellyLib)

add_executable(Pixelly src/Pixelly2.cpp)
target_compile_features(Pixelly PUBLIC cxx_std_20)
target_compile_features(Pixelly PUBLIC c_std_17)
target_link_libraries(Pixelly
    PUBLIC
        allegro_monolith
        Angelscript::angelscript
        effolkronium_random
        fmt::fmt
        PixellyLib)
