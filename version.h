#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2014";
	static const char UBUNTU_VERSION_STYLE[] = "14.06";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 2;
	static const long MINOR = 0;
	static const long BUILD = 1159;
	static const long REVISION = 6188;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2179;
	#define RC_FILEVERSION 2,0,1159,6188
	#define RC_FILEVERSION_STRING "2, 0, 1159, 6188\0"
	static const char FULLVERSION_STRING[] = "2.0.1159.6188";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 59;
	

}
#endif //VERSION_H
