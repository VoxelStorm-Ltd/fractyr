#include "get_version.h"
#include "git_version.h"
#include "version.h"

std::string get_version() {
  /// Return a string describing the current version.
  /// This lives in its own file to minimise re-compilation times, as it's always recompiled
  std::string version(AutoVersion::STATUS);
  if(version.length() != 0) {
    version += " ";
  }
  version += AutoVersion::FULLVERSION_STRING;
  return version;
}

std::string get_git_version() {
  /// Return a string describing the current git revision.
  std::string version(AutoVersion::GIT_BRANCH);
  version += ":";
  version += AutoVersion::GIT_REVISION;
  return version;
}
