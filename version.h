#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1491;
	static const long REVISION  = 7960;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2764;
	#define RC_FILEVERSION 2,3,1491,7960
	#define RC_FILEVERSION_STRING "2, 3, 1491, 7960\0"
	static const char FULLVERSION_STRING [] = "2.3.1491.7960";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 91;
	

}
#endif //VERSION_H
