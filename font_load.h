#ifndef FONT_LOAD_H_INCLUDED
#define FONT_LOAD_H_INCLUDED

/// Font loaders, from file and from memory buffer

#include <string>
#include <FTGL/ftgl.h>

FTFont *font_load(  std::string const &filename, unsigned int size = 16);
FTFont *font_load3d(std::string const &filename, unsigned int size = 16);
FTFont *font_load(  unsigned char const *buffer, size_t buffersize, unsigned int size = 16);
FTFont *font_load3d(unsigned char const *buffer, size_t buffersize, unsigned int size = 16);

#endif // FONT_LOAD_H_INCLUDED
