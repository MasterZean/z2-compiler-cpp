#ifndef _z2c2_Assembly_h_
#define _z2c2_Assembly_h_

#include <Core/Core.h>

using namespace Upp;

#include "ZParser.h"

namespace Z2 {
	
class Overload;

class ZClass: Moveable<ZClass> {
public:
	String Name;
	String Namespace;
	
	Overload& AddOverload();
	
private:
	Vector<Overload> overloads;
};

class Overload: Moveable<Overload> {
public:
	ZClass& Class;
	
	String Name;
	ZParser::Pos EntryPoint;
	
	Overload(ZClass& aClass): Class(aClass) {
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
	
	Vector<ZClass> classes;
};

}

#endif
