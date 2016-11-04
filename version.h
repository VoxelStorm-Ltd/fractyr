#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1483;
	static const long REVISION  = 7915;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2756;
	#define RC_FILEVERSION 2,3,1483,7915
	#define RC_FILEVERSION_STRING "2, 3, 1483, 7915\0"
	static const char FULLVERSION_STRING [] = "2.3.1483.7915";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 83;
	

}
#endif //VERSION_H
