#ifndef _z2clib_Node_h_
#define _z2clib_Node_h_

#include "entities.h"

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
	Overload *ol = nullptr;

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

#endif
