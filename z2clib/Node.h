#ifndef _z2clib_Node_h_
#define _z2clib_Node_h_

#include "entities.h"

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
		ntDef,
		ntList,
		ntConstruct,
		ntPtr,
		ntArray,
		ntSizeOf,
		ntDestruct,
		Property,
		ntDeref,
		Intrinsic,
		ntReturn,
		ntVar,
		ntAlloc,
		ntRawArray,
		ntUsing,
	};
};

class MemNode;

class Node: public ObjectInfo {
public:
	Node* Next = nullptr;
	Node* Prev = nullptr;
	Node* First = nullptr;
	Node* Last = nullptr;

	NodeType::Type NT = NodeType::Invalid;

	bool IsCT = false;
	bool IsLiteral = false;
	bool IsSymbolic = false;
	bool HasSe = false;
	bool LValue = false;

	double DblVal = 0;
	int64  IntVal = 0;
	// TODO: fix
	int reg;

	void SetType(ObjectType* type, ZClass* efType, ZClass* sType) {
		Tt = *type;
		C1 = efType;
		C2 = sType;
	}

	void SetType(ObjectType* type) {
		Tt = *type;
		C1 = type->Class;
		C2 = nullptr;
	}

	void SetType(ObjectType type) {
		Tt = type;
		C1 = type.Class;
		C2 = nullptr;
	}
	
	void SetValue(int i, double d) {
		IntVal = i;
		DblVal = d;
	}

	void Add(Node* node) {
		if (First == NULL) {
			First = node;
			Last = node;
		}
		else {
			node->Prev = Last;
			Last->Next = node;

			Last =  node;
		}
	}

	bool IsZero(Assembly& ass);

	void PromoteToFloatValue(Assembly& ass);
		
	inline MemNode* GetParam() const;
};

class ConstNode: public Node {
public:
	Constant* Const = nullptr;
	int Base = 10;

	ConstNode() {
		NT = NodeType::Const;
	}
};

class ObjectNode: public Node {
public:
	Node* Object = nullptr;
	Node* Parent = nullptr;
};

class MemNode: public ObjectNode {
public:
	String Mem;
	Variable* Var = nullptr;

	bool IsThis = false;
	bool IsThisNop = false;
	bool IsParam = false;
	bool IsLocal = false;
	bool IsClass = false;

	MemNode() {
		NT = NodeType::Memory;
	}
	
	Variable* GetFullMemberAssignment() {
		if (Var && IsClass)
			return Var;
		
		if (Object == nullptr)
			return nullptr;
		
		if (Object->NT != NodeType::Memory)
			return nullptr;
		
		MemNode& mem = *(MemNode*)(Object);
		if (mem.IsThis == false)
			return nullptr;
		
		if (Var == nullptr)
			return nullptr;
		
		return Var;
	}
};

class ReturnNode: public Node {
public:
	Node* Object = nullptr;
	
	ReturnNode() {
		NT = NodeType::ntReturn;
	}
};


class TempNode: public Node {
public:
	Overload *Overload = nullptr;

	TempNode() {
		NT = NodeType::Temporary;
	}
};

class RawArrayNode: public Node {
public:
	Vector<Node*> Array;
	int Ellipsis = -1;
	
	RawArrayNode() {
		NT = NodeType::ntRawArray;
	}
};

class VarNode: public Node {
public:
	Variable* Var = nullptr;

	VarNode() {
		NT = NodeType::ntVar;
	}
};

class CastNode: public Node {
public:
	bool Ptr = false;
	
	CastNode() {
		NT = NodeType::Cast;
	}
};

class ListNode: public Node {
public:
	ListNode() {
		NT = NodeType::ntList;
	}
};

class SizeOfNode: public Node {
public:
	SizeOfNode() {
		NT = NodeType::ntSizeOf;
	}
};

class ArrayNode: public Node {
public:
	ArrayNode() {
		NT = NodeType::ntArray;
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
		opComp = 22,
		opInc = 23,
		opDec = 24,
		opTernary = 25,
	};
	
	Type Op = opNotSet;
	bool Construct = false;
	int Move = 0;
	bool Assign = false;
	
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

class DerefNode: public Node {
public:
	Node* Object = nullptr;
	
	DerefNode() {
		NT = NodeType::ntDeref;
	}
};

class IntNode: public Node {
public:
	enum Type {
		itPtrFree,
		itPtrClone,
		itPtrCloneLim,
		itPtrRealloc,
		itDebugAssert,
		itTraceAssert,
		itPtrAllocVoid,
		itTraceError,
		itDebugError,
		itCArrayCopy,
		itDefaultCons,
		itDefaultCopyCons,
		itDefaultCopy,
		itDefaultMoveCons,
		itDefaultMove,
		itForIndex,
		itForCount,
		itCpuID,
	};
	
	Type I;
	bool NOP = false;

	IntNode() {
		NT = NodeType::Intrinsic;
	}
};

class UsingNode: public Node {
public:
	Vector<Node*> List;
	Vector<String> Name;
	Node* Using = nullptr;
	
	UsingNode() {
		NT = NodeType::ntUsing;
	}
};

class ConstructNode: public Node {
public:
	Overload *Overload = nullptr;

	ConstructNode() {
		NT = NodeType::ntConstruct;
	}
};

class DestructNode: public ConstructNode {
public:
	DestructNode() {
		NT = NodeType::ntDestruct;
	}
};

class AllocNode: public Node {
public:
	ZClass* Class = nullptr;
	Node* Count = nullptr;

	AllocNode() {
		NT = NodeType::ntAlloc;
	}
};

class DefNode: public Node {
public:
	Overload* Overload = nullptr;
	Node* Object = nullptr;
	bool Property = false;
	bool IsDestructor = false;

	DefNode() {
		NT = NodeType::ntDef;
	}
};


class PtrNode: public Node {
public:
	bool Ref = false;
	bool Move = false;
	bool Nop = false;

	PtrNode() {
		NT = NodeType::ntPtr;
	}
};

class PropertyNode: public ObjectNode {
public:
	::Def* Def = nullptr;
	int Qualified = 0;
	ObjectType* FType = nullptr;
	Overload* Overload = nullptr;
	bool UseAsGet = false;

	PropertyNode() {
		NT = NodeType::Property;
	}
};

#endif
