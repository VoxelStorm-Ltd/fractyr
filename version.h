#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1411;
	static const long REVISION  = 7511;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2602;
	#define RC_FILEVERSION 2,3,1411,7511
	#define RC_FILEVERSION_STRING "2, 3, 1411, 7511\0"
	static const char FULLVERSION_STRING [] = "2.3.1411.7511";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 11;
	

}
#endif //VERSION_H
