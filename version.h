#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "03";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2014";
	static const char UBUNTU_VERSION_STYLE[] =  "14.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 1;
	static const long BUILD  = 180;
	static const long REVISION  = 893;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 279;
	#define RC_FILEVERSION 1,1,180,893
	#define RC_FILEVERSION_STRING "1, 1, 180, 893\0"
	static const char FULLVERSION_STRING [] = "1.1.180.893";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 80;
	

}
#endif //VERSION_H
