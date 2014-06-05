#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "05";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2014";
	static const char UBUNTU_VERSION_STYLE[] =  "14.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 8;
	static const long BUILD  = 873;
	static const long REVISION  = 4705;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1539;
	#define RC_FILEVERSION 1,8,873,4705
	#define RC_FILEVERSION_STRING "1, 8, 873, 4705\0"
	static const char FULLVERSION_STRING [] = "1.8.873.4705";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 73;
	

}
#endif //VERSION_H
