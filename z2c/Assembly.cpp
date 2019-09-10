#include "Assembly.h"

namespace Z2 {
	
Assembly::Assembly() {
	CBool = &classes.Add();
	ASSERT(classes.GetCount() == 1);
	CBool->BackendName = "bool";
	
	CSmall = AddCoreInteger("int8", 2);
	CByte = AddCoreInteger("uint8", 3);
	CShort = AddCoreInteger("int16", 4);
	CWord = AddCoreInteger("uint16", 5);
	CInt = AddCoreInteger("int32", 6);
	CDWord = AddCoreInteger("uint32", 7);
	CLong = AddCoreInteger("int64", 8);
	CQWord = AddCoreInteger("uint64", 9);
	
	CFloat = &classes.Add();
	ASSERT(classes.GetCount() == 10);
	CFloat->BackendName = "float";
	CDouble = &classes.Add();
	ASSERT(classes.GetCount() == 11);
	CDouble->BackendName = "double";
	
	CChar = &classes.Add();
	ASSERT(classes.GetCount() == 12);
	CChar->BackendName = "uint32";
	
	CPtrSize = &classes.Add();
	ASSERT(classes.GetCount() == 13);
	CPtrSize->BackendName = "size_t";
	
	CCls = &classes.Add();
	CDef = &classes.Add();
	CVoid = &classes.Add();
	CNull = &classes.Add();
}

ZClass* Assembly::AddCoreInteger(const String& backendName, int count) {
	ZClass& cls = classes.Add();
	ASSERT(classes.GetCount() == count);
	
	cls.BackendName = backendName;
	cls.MIsInteger = true;
	
	return &cls;
}

Overload& ZClass::AddOverload() {
	return Overloads.Add(*this);
}

}