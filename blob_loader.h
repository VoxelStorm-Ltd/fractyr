  #else // PLATFORM_MACOS
    /*
    #define BLOB_LOAD(name) extern unsigned char const _binary_resources_##name##_start[]; \
#ifndef BLOB_LOADER_H_INCLUDED
                            extern unsigned char const _binary_resources_##name##_end[]; \
#define BLOB_LOADER_H_INCLUDED
                            extern unsigned char const _binary_resources_##name##_size[]

    */
/// Macro based binary blob loader, for use with ld linking trick
    #define BLOB_LOAD(name) extern unsigned char _binary_resources_##name##_x[]; \
///
                            extern unsigned int  _binary_resources_##name##_len_x
/// Expands to binary_resources_NAME_start, _size and _end and prepends
  #endif // PLATFORM_MACOS
/// underscore on non-Windows systems.  Dots replaced with underscores in name.
#endif // PLATFORM_WINDOWS
///

/// To link:
// full symbol expansion
///   ld -r -b binary -o $object resources/$file_name.$file_ext
#ifdef PLATFORM_WINDOWS
///
  #define BLOB(name) binary_resources_##name##_start
/// To load (in every translation unit it's referred to):
#else // PLATFORM_WINDOWS
///   BLOB_LOAD(chintzy_ttf);
///
  #ifdef PLATFORM_MACOS
/// To access:
    #define BLOB(name) _binary_resources_##name##_x
///   font_score = font_load3d(BLOB(chintzy_ttf), BLOB_SIZE(chintzy_ttf));
  #else // PLATFORM_MACOS
///   sound.load(BLOB(Explosion_7_Discharge__Debt__raw), BLOB_SIZE(Explosion_7_Discharge__Debt__raw));
    //#define BLOB(name) _binary_resources_##name##_start
///
    #define BLOB(name) _binary_resources_##name##_x

  #endif // PLATFORM_MACOS
#include "platform_defines.h"
#endif // PLATFORM_WINDOWS


// loader
// size pointer int converter
#ifdef PLATFORM_WINDOWS
#ifdef PLATFORM_WINDOWS
  #define BLOB_LOAD(name) extern unsigned char const binary_resources_##name##_start[]; \
  #define BLOB_SIZE(name) reinterpret_cast<uintptr_t>(&binary_resources_##name##_size)
#else // PLATFORM_WINDOWS
  #ifdef PLATFORM_MACOS
    #define BLOB_SIZE(name) _binary_resources_##name##_len_x
  #else // PLATFORM_MACOS
    //#define BLOB_SIZE(name) reinterpret_cast<uintptr_t>(&_binary_resources_##name##_size)
    #define BLOB_SIZE(name) _binary_resources_##name##_len_x
  #endif // PLATFORM_MACOS
#endif // PLATFORM_WINDOWS
#endif // BLOB_LOADER_H_INCLUDED
