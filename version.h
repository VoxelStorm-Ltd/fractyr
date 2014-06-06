#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2014";
	static const char UBUNTU_VERSION_STYLE[] =  "14.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 2;
	static const long BUILD  = 1328;
	static const long REVISION  = 7036;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2463;
	#define RC_FILEVERSION 2,2,1328,7036
	#define RC_FILEVERSION_STRING "2, 2, 1328, 7036\0"
	static const char FULLVERSION_STRING [] = "2.2.1328.7036";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 28;
	

}
#endif //VERSION_H
