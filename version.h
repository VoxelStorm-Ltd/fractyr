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
	static const long MINOR  = 3;
	static const long BUILD  = 345;
	static const long REVISION  = 1790;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 593;
	#define RC_FILEVERSION 1,3,345,1790
	#define RC_FILEVERSION_STRING "1, 3, 345, 1790\0"
	static const char FULLVERSION_STRING [] = "1.3.345.1790";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 45;
	

}
#endif //VERSION_H
