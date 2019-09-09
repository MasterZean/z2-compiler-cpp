#include "Assembly.h"

namespace Z2 {
	
Assembly::Assembly() {
	CCls = &classes.Add();
	CDef = &classes.Add();
	CVoid = &classes.Add();
	CNull = &classes.Add();
	
	CSmall = &classes.Add();
	CSmall->BackendName = "int8";
	CShort = &classes.Add();
	CShort->BackendName = "int16";
	CInt = &classes.Add();
	CInt->BackendName = "int32";
	CLong = &classes.Add();
	CLong->BackendName = "int64";
	
	CByte = &classes.Add();
	CByte->BackendName = "uint8";
	CWord = &classes.Add();
	CWord->BackendName = "uint16";
	CDWord = &classes.Add();
	CDWord->BackendName = "uint32";
	CQWord = &classes.Add();
	CQWord->BackendName = "uint64";
	
	CFloat = &classes.Add();
	CFloat->BackendName = "float";
	CDouble = &classes.Add();
	CDouble->BackendName = "double";
	
	CBool = &classes.Add();
	CBool->BackendName = "bool";
	
	CChar = &classes.Add();
	CChar->BackendName = "uint32";
	
	CPtrSize = &classes.Add();
	CPtrSize->BackendName = "size_t";
}

Overload& ZClass::AddOverload() {
	return Overloads.Add(*this);
}

}