#include "Assembly.h"

namespace Z2 {
	
Assembly::Assembly() {
	CBool = AddCoreNumeric("Bool", "bool", 0);
	
	CSmall = AddCoreInteger("Small", "int8", 1);
	CByte = AddCoreInteger("Byte", "uint8", 2);
	CShort = AddCoreInteger("Short", "int16", 3);
	CWord = AddCoreInteger("Word", "uint16", 4);
	CInt = AddCoreInteger("Int", "int32", 5);
	CDWord = AddCoreInteger("DWord", "uint32", 6);
	CLong = AddCoreInteger("Long", "int64", 7);
	CQWord = AddCoreInteger("QWord", "uint64", 8);
	
	CFloat = AddCoreNumeric("Float", "float", 9);
	CDouble = AddCoreNumeric("Double", "double", 10);
	Classes.Add("");     // 11 is Extended
	
	CChar = AddCoreNumeric("Char", "uint32", 12);
	CPtrSize = AddCoreNumeric("PtrSize", "size_t", 13);
	
	CCls = &AddClass("Class");
	CCls->BackendName = "Class";
	
	CDef = &AddClass("Def");
	
	CVoid = &AddClass("Void");
	CVoid->BackendName = "void";
	
	CNull = &AddClass("Null");
}

ZClass* Assembly::AddCoreNumeric(const String& name, const String& backendName, int index) {
	ASSERT(Classes.GetCount() == index);
	
	ZClass& cls = AddClass(name);
	cls.BackendName = backendName;
	cls.MIsNumeric = true;
	cls.MIndex = index;
	
	return &cls;
}

Method& ZClass::GetAddMethod(const String& name) {
	int i = Methods.Find(name);
	
	if (i == -1)
		return Methods.Add(name, Method(*this, name));
	else
		return Methods[i];
}

Overload& Method::AddOverload() {
	return Overloads.Add(Overload(*this));
}

}