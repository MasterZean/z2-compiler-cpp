#ifndef _z2clib_BaseExprParser_h_
#define _z2clib_BaseExprParser_h_

#include "ZParser.h"
#include "Assembly.h"

class Compiler;
class AST;

class BaseExprParser {
public:
	bool ClassConsts;
	bool InVarMode;
	bool InConstMode;
	bool AllowArrays;

	BaseExprParser(Compiler& aCmp, ZParser& aSrc,  AST& aIrg, Assembly& aAss, bool var = false, bool ct = false): comp(aCmp), irg(aIrg), ass(aAss), parser(aSrc), InVarMode(var), InConstMode(ct) {
		AllowArrays = false;
	}
	
	Node* ParseNumeric(ZClass& conClass, Overload* conOver);
	
	static bool TypesEqualD(Assembly& ass, ObjectType* t1, ObjectType* t2);
	
protected:
	AST& irg;
	Assembly& ass;
	ZParser& parser;
	Compiler& comp;
};

#endif
