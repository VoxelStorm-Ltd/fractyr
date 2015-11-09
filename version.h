#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "26";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1410;
	static const long REVISION  = 7505;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2602;
	#define RC_FILEVERSION 2,3,1410,7505
	#define RC_FILEVERSION_STRING "2, 3, 1410, 7505\0"
	static const char FULLVERSION_STRING [] = "2.3.1410.7505";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 10;
	

}
#endif //VERSION_H
