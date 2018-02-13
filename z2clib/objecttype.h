#ifndef _z2clib_ObjectType_h_
#define _z2clib_ObjectType_h_

#include <Core/Core.h>

using namespace Upp;

class ZClass;
class Assembly;
class Node;

class ObjectType {
public:
	ZClass* Class = nullptr;
	ObjectType* Next = nullptr;
	int Param = 0;
	
	ObjectType() {
	}
	
	ObjectType(ZClass& cls): Class(&cls) {
	}
};

class ObjectInfo: Moveable<ObjectInfo> {
public:
	ObjectType Tt;
	ZClass* C1 = nullptr;
	ZClass* C2 = nullptr;
	
	bool IsRef = false;
	bool IsAddressable = false;
	//bool IsIndirect = false;
	
	bool IsMove = false;
	bool IsConst = false;
	bool IsEfRef = false;
	bool IsEfRefHa = false;

	bool CanAssign(Assembly& ass, ObjectInfo& sec, bool isCt);
	bool CanAssign(Assembly& ass, Node* node);
	
	ObjectInfo() = default;
	
	ObjectInfo(ZClass* cls) {
		Tt.Class = cls;
	}
	
	ObjectInfo(ZClass* cls, bool ref) {
		Tt.Class = cls;
		IsRef = ref;
	}
	
	ObjectInfo(ObjectType* tt) {
		Tt = *tt;
	}
};

inline bool operator==(const ObjectType& Tt, ObjectType* tt) {
	return Tt.Class == tt->Class;
}

inline bool operator==(const ObjectInfo& t1, const ObjectInfo& t2) {
	return t1.Tt.Class == t2.Tt.Class && t1.IsRef == t2.IsRef && /*t1.IsConst == t2.IsConst &&*/ t1.IsMove == t2.IsMove;
}

inline bool operator!=(const ObjectInfo& t1, const ObjectInfo& t2) {
	return !(t1 == t2);
}

class Result: public Moveable<Result> {
public:
	int64 IVal;
	double DVal;
	ObjectInfo I;

	Result() {
	}
};

class Entity: public Result, public Moveable<Entity> {
public:
	enum AccessType {
		atPublic,
		atPrivate,
		atProtected
	};

	enum EntityType {
		etConst,
		etVariable,
	};

	String Name;
	Point Location;
	AccessType Access;
	EntityType Type;
	bool IsStatic;

	Entity() {
		IsStatic = false;
		Access = atPublic;
		Location = Point(-1, -1);
	}
};

#endif
