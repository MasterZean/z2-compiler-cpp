#include "BaseExprParser.h"

#include <z2clibex/Node.h>

Node* BaseExprParser::ParseNumeric(ZClass& conClass, Overload* conOver) {
	Node* exp = nullptr;
	
	int64 oInt;
	double oDub;
	int base;
	int b = parser.ReadInt64(oInt, oDub, base);

	if (b == ZParser::ntInt)
		exp = irg.const_i(oInt, nullptr, base);
	else if (b == ZParser::ntDWord)
		exp = irg.const_u(oInt, nullptr, base);
	else if (b == ZParser::ntLong) {
		exp = irg.const_i(oInt);
		exp->Tt = ass.CLong->Tt;
	}
	else if (b == ZParser::ntQWord) {
		exp = irg.const_u(oInt);
		exp->Tt = ass.CQWord->Tt;
	}
	else if (b == ZParser::ntDouble)
		exp = irg.const_r64(oDub);
	else if (b == ZParser::ntFloat)
		exp = irg.const_r32(oDub);
	else if (b == ZParser::ntPtrSize) {
		exp = irg.const_i(oInt);
		exp->Tt = ass.CPtrSize->Tt;
	}
	else
		ASSERT_(0, "Error in parse int");

	return exp;
}