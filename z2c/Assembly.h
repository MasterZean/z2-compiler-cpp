#ifndef _z2c2_Assembly_h_
#define _z2c2_Assembly_h_

#include <Core/Core.h>

#include "ZParser.h"

namespace Z2 {
	
using namespace Upp;
	
class Overload;
class ZClass;
class Node;
class Method;

class Entity: Moveable<Entity> {
public:
	enum AccessType {
		atPublic,
		atPrivate,
		atProtected
	};
	
	ZParser::Pos EntryPos;
	ZParser::Pos ExitPos;
	
	bool IsStatic = false;
		
	bool IsEvaluated = false;
	AccessType Access = Entity::atPublic;
};

class Variable: public Entity, Moveable<Variable> {
public:
	enum ParamType {
		tyAuto,
		tyRef,
		tyConstRef,
		tyVal,
		tyMove,
	};
		
	String Name;
	
	Point SourcePoint = Point(-1, -1);
	
	bool MIsMember = false;
	int  MIsParam = -1;
	
	bool IsReadOnly = false;
	bool IsCT = false;
	bool InUse = false;
	bool IsValid = false;
	
	ParamType PType = Variable::tyAuto;
	
	ZClass* OwnerClass = nullptr;
	Node* Value = nullptr;
	Node* Default = nullptr;
	ZClass* Class = nullptr;
};

class Block: Moveable<Block> {
public:
	WithDeepCopy<VectorMap<String, Variable*>> Variables;
	
	int Temps = 0;
	bool Returned = false;
	
	void AddVaribleRef(Variable& var) {
		Variables.Add(var.Name, &var);
	}
};

class ZClass: public Entity, Moveable<ZClass> {
public:
	String Name;
	String Namespace;
	String BackendName;
	String GlobalName;
	String MangledName;
	String ParamName;
	
	bool IsModule = false;
	bool IsTemplate = false;
	
	Point SourcePos = Point(-1, -1);
	
	bool MIsInteger = false;
	bool MIsFloat = false;
	bool MIsNumeric = false;
	int  MIndex = -1;
	int  MDecWritten = -1;
	
	Method& GetAddMethod(const String& name);
	
	Variable& AddVariable(const String& name);
	
	ArrayMap<String, Method> Methods;
	ArrayMap<String, Variable> Variables;
};

class Method: Moveable<Method> {
public:
	ZClass& OwnerClass;
	
	String Name;
	String BackendName;
	
	Method(ZClass& aClass, const String& name): OwnerClass(aClass), Name(name) {
	}
	
	Overload& AddOverload();
	Overload& GetOverloadByPoint(const Point& p);
	
	Array<Overload> Overloads;
	Vector<int> OverloadCounts;
};

class Overload: public Entity, Moveable<Overload> {
public:
	ZClass& OwnerClass;
	Method& OwnerMethod;
	
	ZParser::Pos ParamPos;
	ZParser::Pos PostParamPos;
	
	String BackendName;
	
	Point NamePoint = Point(-1, -1);
	Point SourcePoint = Point(-1, -1);
	
	bool IsDestructor = false;
	bool IsVirtual = false;
	bool IsInline = false;
	bool IsConst = false;
	int  IsCons = 0;
	
	int MDecWritten = 0;
	bool IsScanned = false;
	int Score = 0;

	int MinParams = -1;
	ZClass* Return;
	
	String Signature;
	String LogSig;
	String ParamSig;
	
	Array<Variable> Variables;
	ArrayMap<String, Variable> Params;
	VectorMap<String, ZClass*> TParam;
	
	WithDeepCopy<Array<Block>> Blocks;
	
	Vector<Node*> Nodes;
	Index<Overload*> DepOver;
	Index<ZClass*> DepClass;
	
	Overload(Method& aMethod): OwnerClass(aMethod.OwnerClass), OwnerMethod(aMethod), BackendName(aMethod.BackendName) {
	}
	
	Variable& AddVariable() {
		return Variables.Add();
	}
	
	const String& Name() const {
		return OwnerMethod.Name;
	}
};

class SubModule: public ZClass {
public:
	SubModule() {
		IsModule = true;
	}
};

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
	
	Assembly();
	
	ZClass* AddCoreNumeric(const String& name, const String& backendName, const String& mangledName, int index);
	
	ZClass* AddCoreInteger(const String& name, const String& backendName, const String& mangledName, int index) {
		ZClass* cls = AddCoreNumeric(name, backendName, mangledName, index);
		cls->MIsInteger = true;
		return cls;
	}
	
	ZClass& AddClass(const String& name) {
		ZClass& cls = Classes.Add(name);
		cls.Name = name;
		cls.MIndex = Classes.GetCount() - 1;
		return cls;
	}

	bool CanAssign(ZClass* cls, Node* n);
		
	ArrayMap<String, ZClass> Classes;
	ArrayMap<String, SubModule> Modules;
};

}

#endif
