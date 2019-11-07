#ifndef _z2c_Scanner_h_
#define _z2c_Scanner_h_

#include <Core/Core.h>

#include "Assembly.h"
#include "ErrorReporter.h"
#include "tables.h"

namespace Z2 {
	
using namespace Upp;

class ZPackage;

class ZFile: Moveable<ZFile> {
public:
	String Path;
	Time Modified;
	String Data;
	
	void Serialize(Stream& s) {
		s % Path % Modified;
	}
};

class ZSource: public ZFile, Moveable<ZSource> {
public:
	bool IsScaned = false;
	ZPackage* Package = nullptr;
	
	String Namespace;
	SubModule Module;
};

class ZPackage: Moveable<ZPackage> {
public:
	String Name;
	String Path;
	String CachePath;
	
	ArrayMap<String, ZSource> Files;
	
	void Serialize(Stream& s) {
		s % Name % Path % Files;
	}
};

class Scanner {
public:
	enum PlatformType {
		WINDOWS32,
		POSIX,
	};
	
	Scanner(Assembly& aAss): ass(aAss) {
	}
	
	void Scan(ZSource& src);
	void ScanClass(ZClass& conCls, ZParser& parser);
	void ScanDef(ZClass& conCls, ZParser& parser, bool ct, bool st = false);
	void ScanVar(ZClass& conCls, ZParser& parser, bool ct, bool st = false);
	void ScanToken(ZParser& parser);
	
private:
	Assembly& ass;
};

};

#endif
