#ifndef _z2clib_ZFile_h_
#define _z2clib_ZFile_h_

#include <Core/Core.h>

using namespace Upp;

class ZSource;
class ZParser;
class ZClass;

class ZClassAlias: Moveable<ZClassAlias> {
public:
	CParser::Pos Context;
	ZClass* Class = nullptr;
	String Name;
	String Namespace;
	Point Location;
	ZSource* Source = nullptr;
};

class ZClassAliasRef: Moveable<ZClassAliasRef> {
public:
	ZClassAlias* Alias = nullptr;
	int Count = 0;
};

class ZFile: Moveable<ZFile> {
public:
	String Path;
	Time Modified;
	String Data = String::GetVoid();
	
	Index<String> ClassNameList;
	Vector<String> References;
	Vector<Point> ReferencePos;
	VectorMap<String, ZClassAlias> Aliases;
	
	void Serialize(Stream& s) {
		s % Path % Modified % ClassNameList;
	}
};

class ZPackage;
class Assembly;

class ZSource: public ZFile, Moveable<ZSource> {
public:
	bool IsScaned = false;
	bool IsBuilt = false;
	bool SkipNewLines = true;
	ZPackage* Package = nullptr;

	ArrayMap<String, ZClass> ClassPrototypes;

	Vector<ZClass*> Declarations;

	void AddStdClassRefs();
	
	void AddReference(const String& ns);
	
	ZClass& AddClass(const String& name, const String& nameSpace, const Point& pnt);
	
	int FindClassReference(const String& shortName, int start);
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

#endif
