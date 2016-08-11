#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "11";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.08";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1482;
	static const long REVISION  = 7912;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2751;
	#define RC_FILEVERSION 2,3,1482,7912
	#define RC_FILEVERSION_STRING "2, 3, 1482, 7912\0"
	static const char FULLVERSION_STRING [] = "2.3.1482.7912";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 82;
	

}
#endif //VERSION_H
