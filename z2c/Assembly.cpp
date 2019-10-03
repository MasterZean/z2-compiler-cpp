#include "Assembly.h"
#include "Node.h"
#include "tables.h"

namespace Z2 {
	
Assembly::Assembly() {
	CBool = AddCoreNumeric("Bool", "bool", "B", 0);
	
	CSmall = AddCoreInteger("Small", "int8", "o", 1);
	CByte = AddCoreInteger("Byte", "uint8", "b", 2);
	CShort = AddCoreInteger("Short", "int16", "s", 3);
	CWord = AddCoreInteger("Word", "uint16", "w", 4);
	CInt = AddCoreInteger("Int", "int32", "i",  5);
	CDWord = AddCoreInteger("DWord", "uint32", "u", 6);
	CLong = AddCoreInteger("Long", "int64", "l", 7);
	CQWord = AddCoreInteger("QWord", "uint64", "q", 8);
	
	CFloat = AddCoreNumeric("Float", "float", "f", 9);
	CDouble = AddCoreNumeric("Double", "double", "d",  10);
	Classes.Add("");     // 11 is Extended   "e"
	
	CChar = AddCoreNumeric("Char", "uint32", "c", 12);
	CPtrSize = AddCoreNumeric("PtrSize", "size_t", "z", 13);
	
	CCls = &AddClass("Class");
	CCls->BackendName = "Class";
	
	CDef = &AddClass("Def");
	
	CVoid = &AddClass("Void");
	CVoid->BackendName = "void";
	
	CNull = &AddClass("Null");
}

ZClass* Assembly::AddCoreNumeric(const String& name, const String& backendName, const String& mangledName, int index) {
	ASSERT(Classes.GetCount() == index);
	
	ZClass& cls = AddClass(name);
	cls.BackendName = backendName;
	cls.GlobalName = backendName;
	cls.MangledName = mangledName;
	cls.MIsNumeric = true;
	cls.MIndex = index;
	
	return &cls;
}

bool Assembly::CanAssign(ZClass* cls, Node* n) {
	if (cls == CVoid || n->Class == CVoid)
		return false;

	//if (ass.IsPtr(Tt))
	//	return y.Tt.Class == ass.CNull ||
	//		(ass.IsPtr(y.Tt) && (y.Tt.Next->Class == Tt.Next->Class || Tt.Next->Class == ass.CVoid));

	//if (isCt && Tt.Class == ass.CPtrSize && (y.Tt.Class == ass.CInt))
	//	return true;

	if (cls == n->Class)
		return true;
	
	if (cls->MIsNumeric && n->Class->MIsNumeric) {
		ASSERT(n->C1);
		int t1 = cls->MIndex;
		int t2 = n->C1->MIndex;
		ASSERT(t1 >= 0 && t1 <= 13);
		ASSERT(t2 >= 0 && t2 <= 13);
		
		if (TabCanAssign[t1][t2])
			return true;
		else {
			if (n->C2 != NULL) {
				t2 = n->C2->MIndex;
				return TabCanAssign[t1][t2];
			}
			else
				return false;
		}
	}

	return false;
	//return BaseExprParser::TypesEqualDeep(ass, &this->Tt, &y.Tt);
}

Method& ZClass::GetAddMethod(const String& name) {
	int i = Methods.Find(name);
	
	if (i == -1)
		return Methods.Add(name, Method(*this, name));
	else
		return Methods[i];
}

Variable& ZClass::AddVariable(const String& name) {
	int i = Variables.Find(name);
	
	if (i == -1)
		return Variables.Add(name);
	else
		return Variables[i];
}

Overload& Method::AddOverload() {
	return Overloads.Add(Overload(*this));
}

Overload& Method::GetOverloadByPoint(const Point& p) {
	int i = -1;
	
	for (int j = 0; j < Overloads.GetCount(); j++)  {
		if (Overloads[j].NamePoint == p) {
			i = j;
			break;
		}
	}
	ASSERT(i != -1);
	
	return Overloads[i];
}

}