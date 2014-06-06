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
	static const long MINOR = 1;
	static const long BUILD = 1211;
	static const long REVISION = 6458;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2266;
	#define RC_FILEVERSION 2,1,1211,6458
	#define RC_FILEVERSION_STRING "2, 1, 1211, 6458\0"
	static const char FULLVERSION_STRING[] = "2.1.1211.6458";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 11;
	

}
#endif //VERSION_H
