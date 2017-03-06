#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 4;
	static const long BUILD  = 1505;
	static const long REVISION  = 8052;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2788;
	#define RC_FILEVERSION 2,4,1505,8052
	#define RC_FILEVERSION_STRING "2, 4, 1505, 8052\0"
	static const char FULLVERSION_STRING [] = "2.4.1505.8052";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 5;
	

}
#endif //VERSION_H
