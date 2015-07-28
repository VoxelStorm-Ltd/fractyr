#include "font_load.h"
//#include <iostream>
#include <boost/filesystem.hpp>

FTFont *font_load(std::string const &filename, unsigned int size) {
  /// Try to load a font from the specified filename, running all checks first
  if(!(boost::filesystem::exists(filename) &&
       boost::filesystem::is_regular_file(filename))) {
    std::cout << "ERROR: font_load: no such file as " << filename << std::endl;
    return nullptr;
  }
  //FTFont *font = new FTBitmapFont(filename.c_str());
  //FTFont *font = new FTPixmapFont(filename.c_str());
  FTFont *font = new FTTextureFont(filename.c_str());
  //FTFont *font = new FTBufferFont(filename.c_str());
  if(!font) {
    std::cout << "ERROR: font_load: could not load font from file " << filename << std::endl;
    return nullptr;
  }
  font->CharMap(ft_encoding_unicode);
  font->FaceSize(size, 72);  // points, display resolution: 10, 72 = 10PPEm, ignored for anything other than a buffer font
  font->UseDisplayList(false);
  return font;
}

FTFont *font_load(unsigned char const *buffer, size_t buffersize, unsigned int size) {
  /// Try to load a font from the specified filename, running all checks first
  FTFont *font = new FTTextureFont(buffer, buffersize);
  //FTFont *font = new FTBufferFont(buffer, buffersize);
  if(!font) {
    std::cout << "ERROR: font_load: could not load font from memory" << std::endl;
    return nullptr;
  }
  font->CharMap(ft_encoding_unicode);
  font->FaceSize(size, 72);  // points, display resolution: 10, 72 = 10PPEm, ignored for anything other than a buffer font
  font->UseDisplayList(false);
  return font;
}

FTFont *font_load3d(std::string const &filename, unsigned int size) {
  /// Try to load a font from the specified filename as a font for 3D display, running all checks first
  if(!(boost::filesystem::exists(filename) &&
       boost::filesystem::is_regular_file(filename))) {
    std::cout << "ERROR: font_load: no such file as " << filename << std::endl;
    return nullptr;
  }
  //FTFont *font = new FTPolygonFont(filename.c_str());
  FTFont *font = new FTExtrudeFont(filename.c_str());
  if(!font) {
    std::cout << "ERROR: font_load: could not load font from file " << filename << std::endl;
    return nullptr;
  }
  font->CharMap(ft_encoding_unicode);
  font->FaceSize(size, 72);  // points, display resolution: 10, 72 = 10PPEm, ignored for anything other than a buffer font
  font->UseDisplayList(false);
  font->Depth(1.0);
  return font;
}

FTFont *font_load3d(unsigned char const *buffer, size_t buffersize, unsigned int size) {
  /// Try to load a font from the specified filename as a font for 3D display, running all checks first
  FTFont *font = new FTExtrudeFont(buffer, buffersize);
  if(!font) {
    std::cout << "ERROR: font_load: could not load font from memory" << std::endl;
    return nullptr;
  }
  font->CharMap(ft_encoding_unicode);
  font->FaceSize(size, 72);  // points, display resolution: 10, 72 = 10PPEm, ignored for anything other than a buffer font
  font->UseDisplayList(false);
  font->Depth(1.0);
  return font;
}
