#ifndef _z2clib_BaseCompiler_h_
#define _z2clib_BaseCompiler_h_

#include "Assembly.h"
#include "Source.h"

class BaseCompiler {
public:
	enum PlatformType {
		WINDOWS32,
		POSIX,
	};
	
	BaseCompiler(Assembly& aAss): ass(aAss) {
#ifdef PLATFORM_WIN32
		Platform = WINDOWS32;
#endif

#ifdef PLATFORM_POSIX
		Platform = POSIX;
#endif
	}
	
	VectorMap<String, String> LookUp;
	PlatformType Platform;
	
	int LookUpClass(ZSource& source, const String& className);
	int LookUpClassInReferences(ZSource& source, const String& className);
	int LookUpQualifiedClass(const String& className);
		
	ZSource& LoadSource(ZSource& source);
	ZSource* FindSource(const String& aSourcePath);
	
	Assembly& GetAssembly() {
		return ass;
	}
	
protected:
	Assembly& ass;
	
	ArrayMap<String, ZPackage> packages;
	int filesOpened = 0;
};

#endif
