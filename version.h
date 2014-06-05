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
	static const long BUILD  = 844;
	static const long REVISION  = 4552;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1463;
	#define RC_FILEVERSION 1,8,844,4552
	#define RC_FILEVERSION_STRING "1, 8, 844, 4552\0"
	static const char FULLVERSION_STRING [] = "1.8.844.4552";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 44;
	

}
#endif //VERSION_H
