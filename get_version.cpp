#include "get_version.h"
#include "version.h"

std::string const get_version() {
  /// Return a string describing the current version.
  /// This lives in its own file to minimise re-compilation times, as it's always recompiled
  std::string version(AutoVersion::STATUS);
  if(version.length() != 0) {
    version += " ";
  }
  version += AutoVersion::FULLVERSION_STRING;
  return version;
}
