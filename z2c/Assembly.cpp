#include "Assembly.h"

namespace Z2 {
	
Assembly::Assembly() {
	CBool = &classes.Add();
	ASSERT(classes.GetCount() == 1);
	CBool->BackendName = "bool";
	
	CSmall = &classes.Add();
	ASSERT(classes.GetCount() == 2);
	CSmall->BackendName = "int8";
	CByte = &classes.Add();
	ASSERT(classes.GetCount() == 3);
	CByte->BackendName = "uint8";
	
	CShort = &classes.Add();
	ASSERT(classes.GetCount() == 4);
	CShort->BackendName = "int16";
	CWord = &classes.Add();
	ASSERT(classes.GetCount() == 5);
	CWord->BackendName = "uint16";
	
	CInt = &classes.Add();
	ASSERT(classes.GetCount() == 6);
	CInt->BackendName = "int32";
	CDWord = &classes.Add();
	ASSERT(classes.GetCount() == 7);
	CDWord->BackendName = "uint32";
	
	CLong = &classes.Add();
	ASSERT(classes.GetCount() == 8);
	CLong->BackendName = "int64";
	CQWord = &classes.Add();
	ASSERT(classes.GetCount() == 9);
	CQWord->BackendName = "uint64";
	
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

Overload& ZClass::AddOverload() {
	return Overloads.Add(*this);
}

}