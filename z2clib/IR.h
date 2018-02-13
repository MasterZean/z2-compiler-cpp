#ifndef _z2clib_IR_h_
#define _z2clib_IR_h_

#include "Node.h"

template<class Tt>
class NodePool {
private:
	Vector<Tt*> nodes;
	int count;

public:
	NodePool() {
		count = 0;
	}

	~NodePool() {
		for (int i = 0; i < nodes.GetCount(); i++)
			delete nodes[i];
	}

	inline Tt* Get() {
		Tt* node;

		if (count >= nodes.GetCount())
			node = nodes.Add(new Tt);
		else {
			node = nodes[count];
			*node = Tt();
		}

		count += 1;

		return node;
		//return new Tt();
	}

	inline void Clear() {
		count = 0;
	}
};

class Assembly;
class Compiler;

class IR {
protected:
	Assembly& ass;
		
	NodePool<OpNode> opNodes;
	NodePool<UnaryOpNode> unaryOpNodes;
	NodePool<ConstNode> constNodes;
	NodePool<DerefNode> derefNodes;
	NodePool<ListNode> listNodes;
	NodePool<SizeOfNode> sizeOfNodes;
	NodePool<CastNode> castNodes;
	NodePool<ReturnNode> retNodes;
	NodePool<AllocNode> allocNodes;
	
public:
	bool FoldConstants = true;
	Overload *Over = nullptr;
	Compiler* Comp = nullptr;
	
	IR(Assembly& ass): ass(ass) {
	}
	
	Assembly& GetAssembly() const {
		return ass;
	}
	
	void fillSignedTypeInfo(int64 l, Node* node, ZClass* cls = nullptr);
	void fillUnsignedTypeInfo(uint64 l, Node* node, ZClass* cls = nullptr);

	ConstNode* const_i(int64 l, ZClass* cls = nullptr, int base = 10);
	ConstNode* const_u(uint64 l, ZClass* cls = nullptr, int base = 10);
	ConstNode* const_r32(double l);
	ConstNode* const_r64(double l);
	ConstNode* const_bool(bool l);
	ConstNode* const_char(int l, int base = 10);
	ConstNode* const_void();
	ConstNode* const_null();
	ConstNode* const_str(int index);
	
	Node* opArit(Node* left, Node* right, OpNode::Type op, const Point& p);
	Node* opRel(Node* left, Node* right, OpNode::Type op, const Point& p);
	Node* opLog(Node* left, Node* right, OpNode::Type op);
	
	Node* op_bitand(Node* left, Node* right);
	Node* op_bitor(Node* left, Node* right);
	Node* op_bitxor(Node* left, Node* right);
	
	Node* minus(Node* node);
	Node* plus(Node* node);
	Node* op_not(Node* node);
	Node* bitnot(Node* node);
	Node* inc(Node* node, bool prefix = false);
	Node* dec(Node* node, bool prefix = false);
	
	Node* opTern(Node* cond, Node* left, Node* right);
	
	Node* add(Node* left, Node* right, const Point& p) {
		return opArit(left, right, OpNode::opAdd, p);
	}

	Node* sub(Node* left, Node* right, const Point& p) {
		return opArit(left, right, OpNode::opSub, p);
	}

	Node* mul(Node* left, Node* right, const Point& p) {
		return opArit(left, right, OpNode::opMul, p);
	}

	Node* div(Node* left, Node* right, const Point& p) {
		return opArit(left, right, OpNode::opDiv, p);
	}

	Node* mod(Node* left, Node* right, const Point& p) {
		return opArit(left, right, OpNode::opMod, p);
	}
	
	Node* eq(Node* left, Node* right, const Point& p)  {
		return opRel(left, right, OpNode::opEq, p);
	}

	Node* neq(Node* left, Node* right, const Point& p) {
		return opRel(left, right, OpNode::opNeq, p);
	}
	
	Node* less(Node* left, Node* right, const Point& p) {
		return opRel(left, right, OpNode::opLess, p);
	}

	Node* lessEq(Node* left, Node* right, const Point& p) {
		return opRel(left, right, OpNode::opLessEq, p);
	}

	Node* more(Node* left, Node* right, const Point& p) {
		return opRel(left, right, OpNode::opMore, p);
	}

	Node* moreEq(Node* left, Node* right, const Point& p) {
		return opRel(left, right, OpNode::opMoreEq, p);
	}
		
	Node* shl(Node* left, Node* right, const Point& p);
	Node* shr(Node* left, Node* right, const Point& p);
	
	Node* deref(Node* node);
	Node* list(Node* node);
	Node* size(ZClass& cls);
	Node* cast(Node* left, ObjectType* tt, bool sc = true, bool ptr = false);
	
	ReturnNode* ret(Node* node);

	AllocNode* alloc(ZClass* cls, Node* count);
	
	Node* op(Node* left, Node* right, OpNode::Type op, const Point& p) {
		if (op <= OpNode::opMod)
			return opArit(left, right, op, p);
		else if (op <= OpNode::opShl)
			return shl(left, right, p);
		else if (op <= OpNode::opShr)
			return shr(left, right, p);
		else if (op <= OpNode::opNeq)
			return opRel(left, right, op, p);
		else if (op <= OpNode::opBitAnd)
			return op_bitand(left, right);
		else if (op <= OpNode::opBitXor)
			return op_bitxor(left, right);
		else if (op <= OpNode::opBitOr)
			return op_bitor(left, right);
		else if (op <= OpNode::opLogOr)
			return opLog(left, right, op);
		else {
			ASSERT(0);
			return nullptr;
		}
	}
};


#endif
