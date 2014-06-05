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
	static const long MINOR  = 10;
	static const long BUILD  = 1041;
	static const long REVISION  = 5566;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1922;
	#define RC_FILEVERSION 1,10,1041,5566
	#define RC_FILEVERSION_STRING "1, 10, 1041, 5566\0"
	static const char FULLVERSION_STRING [] = "1.10.1041.5566";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 41;
	

}
#endif //VERSION_H
