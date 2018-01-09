#ifndef BLOB_LOADER_H_INCLUDED
#define BLOB_LOADER_H_INCLUDED

/// Macro based binary blob loader, for use with ld linking trick
///
/// Expands to binary_resources_NAME_start, _size and _end and prepends
/// underscore on non-Windows systems.  Dots replaced with underscores in name.
///
/// To link:
///   ld -r -b binary -o $object resources/$file_name.$file_ext
///
/// To load (in every translation unit it's referred to):
///   BLOB_LOAD(chintzy_ttf);
///
/// To access:
///   font_score = font_load3d(BLOB(chintzy_ttf), BLOB_SIZE(chintzy_ttf));
///   sound.load(BLOB(Explosion_7_Discharge__Debt__raw), BLOB_SIZE(Explosion_7_Discharge__Debt__raw));
///

#include "platform_defines.h"

/// Loader
#ifdef PLATFORM_WINDOWS
  #define BLOB_LOAD(name) extern unsigned char const binary_resources_##name##_start[]; \
                          extern unsigned char const binary_resources_##name##_end[]; \
                          extern unsigned char const binary_resources_##name##_size[]
#else // PLATFORM_WINDOWS
  #ifdef PLATFORM_MACOS
    #define BLOB_LOAD(name) extern unsigned char _binary_resources_##name##_x[]; \
                            extern unsigned int  _binary_resources_##name##_len_x
  #else // PLATFORM_MACOS
    /*
    #define BLOB_LOAD(name) extern unsigned char const _binary_resources_##name##_start[]; \
                            extern unsigned char const _binary_resources_##name##_end[]; \
                            extern unsigned char const _binary_resources_##name##_size[]
    */
    #define BLOB_LOAD(name) extern unsigned char _binary_resources_##name##_x[]; \
                            extern unsigned int  _binary_resources_##name##_len_x
  #endif // PLATFORM_MACOS
#endif // PLATFORM_WINDOWS

/// Full symbol expansion
#ifdef PLATFORM_WINDOWS
  #define BLOB(name) binary_resources_##name##_start
#else // PLATFORM_WINDOWS
  #ifdef PLATFORM_MACOS
    #define BLOB(name) _binary_resources_##name##_x
  #else // PLATFORM_MACOS
    //#define BLOB(name) _binary_resources_##name##_start
    #define BLOB(name) _binary_resources_##name##_x
  #endif // PLATFORM_MACOS
#endif // PLATFORM_WINDOWS

/// Size pointer int converter
#ifdef PLATFORM_WINDOWS
  #define BLOB_SIZE(name) reinterpret_cast<uintptr_t>(&binary_resources_##name##_size)
#else // PLATFORM_WINDOWS
  #ifdef PLATFORM_MACOS
    #define BLOB_SIZE(name) _binary_resources_##name##_len_x
  #else // PLATFORM_MACOS
    //#define BLOB_SIZE(name) reinterpret_cast<uintptr_t>(&_binary_resources_##name##_size)
    #define BLOB_SIZE(name) _binary_resources_##name##_len_x
  #endif // PLATFORM_MACOS
#endif // PLATFORM_WINDOWS

/// Convenience function to load as a string
#define STRING_BLOB(name) std::string(reinterpret_cast<char const*>(BLOB(name)), BLOB_SIZE(name))

#endif // BLOB_LOADER_H_INCLUDED
