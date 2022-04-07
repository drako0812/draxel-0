###############################################################################
#
# TARGET SETTINGS
#
###############################################################################

if(NOT DEFINED PXLY_LIB_SHARED)
    set(PXLY_LIB_SHARED TRUE CACHE BOOL "Build Pixelly's Library as a shared library?")
endif()

###############################################################################
#
# SET UP TARGETS
#
###############################################################################
set(PXLY_LIB_SRCS
    src/base.cpp
    src/machine.cpp
    src/gpu.cpp
    src/input.cpp)

if(PXLY_LIB_SHARED)
    add_library(PixellyLib SHARED
        ${PXLY_LIB_SRCS})
else()
    add_library(PixellyLib STATIC
        ${PXLY_LIB_SRCS})
endif()
target_include_directories(PixellyLib
    PUBLIC
        ./include
        ../deps/allegro5/include
        ../deps/allegro5/addons/acodec
        ../deps/allegro5/addons/audio
        ../deps/allegro5/addons/color
        ../deps/allegro5/addons/font
        ../deps/allegro5/addons/image
        ../deps/allegro5/addons/native_dialog
        ../deps/allegro5/addons/primitives
        ../deps/allegro5/addons/ttf
        ../deps/allegro5/addons/video
        ${CMAKE_BINARY_DIR}/deps/allegro5/include)
target_compile_features(PixellyLib PUBLIC cxx_std_20)
target_compile_features(PixellyLib PUBLIC c_std_17)
target_link_libraries(PixellyLib
    PUBLIC
        allegro_monolith
        Angelscript::angelscript
        effolkronium_random
        fmt::fmt)
