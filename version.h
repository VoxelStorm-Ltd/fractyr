#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "22";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 4;
	static const long BUILD  = 1538;
	static const long REVISION  = 8229;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 2823;
	#define RC_FILEVERSION 2,4,1538,8229
	#define RC_FILEVERSION_STRING "2, 4, 1538, 8229\0"
	static const char FULLVERSION_STRING [] = "2.4.1538.8229";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 38;
	

}
#endif //VERSION_H
