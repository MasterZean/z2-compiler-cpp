#ifndef _z2c2_Node_h_
#define _z2c2_Node_h_

using namespace Upp;

#include "ZParser.h"

namespace Z2 {

class ZClass;
class Variable;

class NodeType {
public:
	enum Type {
		Invalid,
		Const,
		BinaryOp,
		UnaryOp,
		Memory,
		Cast,
		Temporary,
		Def,
		List,
		Construct,
		Ptr,
		Index,
		SizeOf,
		Destruct,
		Property,
		Deref,
		Intrinsic,
		Return,
		Var,
		Alloc,
		Array,
		Using,
	};
};
	
class Node: Moveable<Node> {
public:
	NodeType::Type NT = NodeType::Invalid;
	
	ZClass* Class = nullptr;
	ZClass* C1 = nullptr;
	ZClass* C2 = nullptr;
	ZClass* C3 = nullptr;
	
	bool IsConst = false;
	bool IsCT = false;
	bool IsLiteral = false;
	bool IsTemporary = false;
	
	bool IsAddressable = false;
	
	bool HasSe = false;
	
	int64  IntVal = 0;
	double DblVal = 0;

	void SetClass(ZClass& cls) {
		Class = &cls;
	}
	
	void SetClass(ZClass* cls) {
		Class = cls;
	}
};

class ConstNode: public Node {
public:
	int Base = 10;

	ConstNode() {
		NT = NodeType::Const;
	}
};


class VarNode: public Node {
public:
	Variable* Var = nullptr;

	VarNode() {
		NT = NodeType::Var;
	}
};

class MemNode: public Node {
public:
	Variable* Var = nullptr;

	MemNode() {
		NT = NodeType::Memory;
	}
};

}

#endif
