#ifndef _z2c2_Node_h_
#define _z2c2_Node_h_

#include <Core/Core.h>

#include "ZParser.h"

namespace Z2 {
	
using namespace Upp;

class ZClass;
class Variable;
class Assembly;
class Overload;

class NodeType {
public:
	enum Type {
		Invalid,
		Const,
		BinaryOp,
		UnaryOp,
		Memory,
		Assign,
		Cast,
		//Temporary,
		Call,
		List,
		/*Construct,
		Ptr,
		Index,
		SizeOf,
		Destruct,
		Property,
		Deref,
		Intrinsic,*/
		Return,
		Var,
		/*Alloc,
		Array,*/
		Using,
		Block,
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
	
	int OriginalLine = 0;

	void SetType(ZClass& cls) {
		Class = &cls;
		C1 = Class;
	}
	
	void SetType(ZClass* cls) {
		Class = cls;
		C1 = Class;
	}
	
	void SetType(ZClass* cls, ZClass* e1, ZClass* e2 = nullptr) {
		Class = cls;
		C1 = e1;
		C2 = e2;
	}
	
	bool IsZero(Assembly& ass);
	void PromoteToFloatValue(Assembly& ass);
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

class BlockNode: public Node {
public:
	BlockNode() {
		NT = NodeType::Block;
	}
};

class OpNode: public Node {
public:
	enum Type {
		opNotSet = -1,
		opAdd = 0,
		opSub = 1,
		opMul = 2,
		opDiv = 3,
		opMod = 4,
		opShl = 5,
		opShr = 6,
		opLess = 7,
		opLessEq = 8,
		opMore   = 9,
		opMoreEq = 10,
		opEq = 11,
		opNeq = 12,
		opBitAnd = 13,
		opBitXor = 14,
		opBitOr = 15,
		opLogAnd = 16,
		opLogOr = 17,
		opAssign = 18,
		opPlus = 19,
		opMinus = 20,
		opNot = 21,
		opBitNot = 22,
		opInc = 23,
		opDec = 24,
		opTernary = 25,
	};
	
	Type Op = opNotSet;
	
	Node* OpA = nullptr;
	Node* OpB = nullptr;
	Node* OpC = nullptr;

	OpNode() {
		NT = NodeType::BinaryOp;
	}
};

class UnaryOpNode: public OpNode {
public:
	bool Prefix = false;
	
	UnaryOpNode() {
		NT = NodeType::UnaryOp;
	}
};

class CastNode: public Node {
public:
	Node* Object = nullptr;
	ZClass* Class = nullptr;

	CastNode() {
		NT = NodeType::Cast;
	}
};

class CallNode: public Node {
public:
	Overload* Over = nullptr;
	Vector<Node*> Params;

	CallNode() {
		NT = NodeType::Call;
	}
};

class RetNode: public Node {
public:
	Node* Value = nullptr;
	
	RetNode() {
		NT = NodeType::Return;
	}
};

class AssignNode: public Node {
public:
	Node* LS = nullptr;
	Node* RS = nullptr;
	
	OpNode::Type Op = OpNode::opNotSet;
	
	AssignNode() {
		NT = NodeType::Assign;
	}
};

class ListNode: public Node {
public:
	Vector<Node*> Params;

	ListNode() {
		NT = NodeType::List;
	}
};

}

#endif
