#include "Assembly.h"

namespace Z2 {
	
Assembly::Assembly() {
	CCls = &classes.Add();
	CDef = &classes.Add();
	CVoid = &classes.Add();
	CNull = &classes.Add();
	CInt = &classes.Add();
	CDWord = &classes.Add();
	CBool = &classes.Add();
	CByte = &classes.Add();
	CSmall = &classes.Add();
	CShort = &classes.Add();
	CWord = &classes.Add();
	CFloat = &classes.Add();
	CDouble = &classes.Add();
	CChar = &classes.Add();
	CLong = &classes.Add();
	CQWord = &classes.Add();
	CPtrSize = &classes.Add();
}

Overload& ZClass::AddOverload() {
	return overloads.Add(*this);
}

}