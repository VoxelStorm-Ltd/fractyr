#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2014";
	static const char UBUNTU_VERSION_STYLE[] =  "14.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 5;
	static const long BUILD  = 596;
	static const long REVISION  = 3165;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1029;
	#define RC_FILEVERSION 1,5,596,3165
	#define RC_FILEVERSION_STRING "1, 5, 596, 3165\0"
	static const char FULLVERSION_STRING [] = "1.5.596.3165";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 96;
	

}
#endif //VERSION_H
