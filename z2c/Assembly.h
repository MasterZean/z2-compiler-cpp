#ifndef _z2c2_Assembly_h_
#define _z2c2_Assembly_h_

#include <Core/Core.h>

using namespace Upp;

#include "ZParser.h"

namespace Z2 {
	
class Overload;
class ZClass;
class Node;

class Variable: Moveable<Variable> {
public:
	String Name;
	Point SourcePos = Point(-1, -1);
	
	ZClass& OwnerClass;
	Node* Value = nullptr;
	
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
	
	Overload& AddOverload();
	
	Array<Overload> Overloads;
	Array<Variable> Variables;
};

class Overload: Moveable<Overload> {
public:
	ZClass& OwnerClass;
	
	String Name;
	ZParser::Pos EntryPoint;
	Point SourcePos = Point(-1, -1);
	
	Array<Variable> Variables;
	Array<Variable> Params;
	
	WithDeepCopy<Array<Block>> Blocks;
	
	Overload(ZClass& aClass): OwnerClass(aClass) {
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
	
private:
	
	Array<ZClass> classes;
};

}

#endif
