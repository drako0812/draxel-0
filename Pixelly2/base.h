#pragma once

#if defined(__cplusplus)
#    if defined(PXLY_SHARED)
#        if defined(WINDOWS)
#            if defined(PXLY_EXPORT)
#                define PXLY_API __declspec(dllexport) extern "C"
#            else
#                define PXLY_API __declspec(dllimport) extern "C"
#            endif
#        else
#            define PXLY_API extern "C"
#        endif
#    else
#        define PXLY_API extern "C"
#    endif
#else
#    if defined(PXLY_SHARED)
#        if defined(WINDOWS)
#            if defined(PXLY_EXPORT)
#                define PXLY_API __declspec(dllexport)
#            else
#                define PXLY_API __declspec(dllimport)
#            endif
#        else
#            define PXLY_API
#        endif
#    else
#        define PXLY_API
#    endif
#endif
