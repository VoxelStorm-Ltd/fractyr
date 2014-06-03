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
	static const long MINOR  = 2;
	static const long BUILD  = 258;
	static const long REVISION  = 1329;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 431;
	#define RC_FILEVERSION 1,2,258,1329
	#define RC_FILEVERSION_STRING "1, 2, 258, 1329\0"
	static const char FULLVERSION_STRING [] = "1.2.258.1329";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 58;
	

}
#endif //VERSION_H
