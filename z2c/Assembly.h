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

class Variable: Moveable<Variable> {
public:
	String Name;
	Point SourcePos = Point(-1, -1);
	
	bool MIsMember = false;
	
	ZClass& OwnerClass;
	Node* Value = nullptr;
	ZClass* Class = nullptr;
	
	Variable(ZClass& aClass): OwnerClass(aClass) {
	}
};

class Block: Moveable<Block> {
public:
	WithDeepCopy<VectorMap<String, Variable*>> Variables;
	
	int Temps = 0;
	
	void AddVaribleRef(Variable& var) {
		Variables.Add(var.Name, &var);
	}
};

class ZClass: Moveable<ZClass> {
public:
	String Name;
	String Namespace;
	String BackendName;
	
	Point SourcePos = Point(-1, -1);
	
	bool MIsInteger = false;
	bool MIsNumeric = false;
	int  MIndex = -1;
	
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
	
	Array<Overload> Overloads;
};

class Overload: Moveable<Overload> {
public:
	ZClass& OwnerClass;
	Method& OwnerMethod;
	
	ZParser::Pos EntryPoint;
	Point SourcePos = Point(-1, -1);
	
	bool IsDestructor = false;
	bool IsVirtual = false;
	bool IsInline = false;
	
	int MDecWritten = 0;
	
	ZClass* Return;
	
	Array<Variable> Variables;
	Array<Variable> Params;
	
	WithDeepCopy<Array<Block>> Blocks;
	
	Vector<Node*> Nodes;
	Vector<Overload*> DepOver;
	
	Overload(Method& aMethod): OwnerClass(aMethod.OwnerClass), OwnerMethod(aMethod) {
	}
	
	Variable& AddVariable() {
		return Variables.Add(Variable(OwnerClass));
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
	
	ZClass* AddCoreNumeric(const String& name, const String& backendName, int index);
	
	ZClass* AddCoreInteger(const String& name, const String& backendName, int index) {
		ZClass* cls = AddCoreNumeric(name, backendName, index);
		cls->MIsInteger = true;
		return cls;
	}
	
	ZClass& AddClass(const String& name) {
		ZClass& cls = Classes.Add(name);
		cls.Name = name;
		return cls;
	}

	ArrayMap<String, ZClass> Classes;
};

}

#endif
