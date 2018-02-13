#ifndef _z2c_Assembly_h_
#define _z2c_Assembly_h_

#include <Core/Core.h>

using namespace Upp;

#include "ZParser.h"
#include "entities.h"
#include "Source.h"

class Compiler;

class Assembly {
public:
	ZClass* CCls = nullptr;
	ZClass* CDef = nullptr;
	ZClass* CVoid = nullptr;
	ZClass* CNull = nullptr;
	ZClass* CInt = nullptr;
	ZClass* CDWord = nullptr;
	ZClass* CBool = nullptr;
	ZClass* CByte = nullptr;
	ZClass* CSmall = nullptr;
	ZClass* CShort = nullptr;
	ZClass* CWord = nullptr;
	ZClass* CFloat = nullptr;
	ZClass* CDouble = nullptr;
	ZClass* CChar = nullptr;
	ZClass* CLong = nullptr;
	ZClass* CQWord = nullptr;
	ZClass* CPtrSize = nullptr;
	ZClass* CPtr = nullptr;
	ZClass* CString = nullptr;
	ZClass* CRaw = nullptr;
	ZClass* CStream = nullptr;
	ZClass* CIntrinsic = nullptr;
	ZClass* CSlice = nullptr;
	ZClass* CVect = nullptr;

	ArrayMap<String, ZClass> Classes;
	Index<String> StringConsts;
	VectorMap<String, int> ClassCounts;
	ArrayMap<String, ZClassAlias> Aliases;

	String TypeToString(ObjectType* type);
	String TypeToString(ObjectInfo* type, bool qual = true);
	String BTypeToString(ObjectInfo* type, bool qual = true);
	String CTypeToString(ObjectInfo* type, bool qual = true);
	String ClassToString(ObjectInfo* type, bool qual = true);
	
	void AddBuiltInClasses();

	bool IsNumeric(const ObjectType& type) const {
		return type.Class->MIsNumeric;
	}
	
	bool IsInteger(const ObjectType& type) const {
		return type.Class->MIsInteger;
	}

	bool IsPtr(const ObjectType& ot) {
		return ot.Class == CPtr;
	}
	
	bool IsSignedInt(ObjectType* type) const;
	bool IsSignedInt(const ObjectType& type) const;
	bool IsFloat(ObjectType* type) const;
	bool IsFloat(const ObjectType& type) const;
	bool IsUnsignedInt(ObjectType* type) const;
	bool IsUnsignedInt(const ObjectType& type) const;
	
	ZClass& AddClass(const String& name, const String& nameSpace, ZParser& parser, const Point& pnt);

	ObjectType GetPtr(ObjectType* sub) {
		ObjectType obj;
		obj.Class = CPtr;
		obj.Param = 0;
		obj.Next = sub;

		return obj;
	}

	void AddClassCount(const String& cls) {
		int index = ClassCounts.Find(cls);
		
		if (index == -1)
			ClassCounts.Add(cls, 1);
		else
			ClassCounts[index]++;
	}
	
	int AddStringConst(const String& str) {
		return StringConsts.FindAdd(str);
	}

	ZClass& GetClass(const String& name, ZParser& parser) {
		Point p = parser.GetPoint();
		int j = Classes.Find(name);
		if (j == -1)
			parser.Error(p, "undefined class: '\f" + name + "\f'");
		return Classes[j];
	}

	ZClass& Clone(ZClass& cls, const String& name, const String& bname);
	
	void AddSource(ZSource& src);
	
	void SetOwnership(ZClass& tclass);

private:
	ZClass* AddCoreType(const String& ns, const String& name, const String& backendName, bool num = false, bool integer = false, bool core = true);
	
	void TypeToString(const ObjectType& type, String& str);
};

#endif
