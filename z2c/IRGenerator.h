#ifndef _z2c2_IRGenerator_h_
#define _z2c2_IRGenerator_h_

#include <Core/Core.h>

#include "Node.h"

namespace Z2 {
	
using namespace Upp;
	
class Assembly;
	
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

class IRGenerator {
public:
	IRGenerator(Assembly& aAss): ass(aAss) {
	}
	
	void fillSignedTypeInfo(int64 l, Node* node, ZClass* cls = nullptr);
	void fillUnsignedTypeInfo(uint64 l, Node* node, ZClass* cls = nullptr);

	ConstNode* constIntSigned(int64 l, int base, ZClass* cls = nullptr);
	
	ConstNode* constIntSigned(int64 l, ZClass* cls = nullptr) {
		return constIntSigned(l, 10, cls);
	}
	
	ConstNode* constIntUnsigned(uint64 l, int base = 10, ZClass* cls = nullptr);
	
	ConstNode* constIntUnsigned(int64 l, ZClass* cls = nullptr) {
		return constIntUnsigned(l, 10, cls);
	}
	
	ConstNode* constFloatSingle(double l);
	ConstNode* constFloatDouble(double l);
	
	ConstNode* constChar(int l, int base = 10);
	ConstNode* constBool(bool l);
	ConstNode* constClass(ZClass* cls);
	ConstNode* constNull();
	ConstNode* constVoid();
	
	CastNode* cast(Node* object, ZClass* cls);
	
	BlockNode* openBlock();
	BlockNode* closeBlock();
	
	VarNode* defineLocalVar(Variable& v);
	MemNode* mem(Variable& v);
	
	AssignNode* assign(Node* ls, Node* rs);
	
	Node* op(Node* left, Node* right, OpNode::Type op, const Point& p);
	Node* opArit(Node* left, Node* right, OpNode::Type op, const Point& p);
	Node* opShl(Node* left, Node* right, const Point& p);
	Node* opShr(Node* left, Node* right, const Point& p);
	Node* opRel(Node* left, Node* right, OpNode::Type op, const Point& p);
	
	Node* opAritCT(Node* left, Node* right, OpNode::Type op, ZClass* cls, ZClass* e, int64& dInt, double& dDouble);
	Node* opRelCT(Node* left, Node* right, OpNode::Type op, ZClass* e);
	
	Node* opBitAnd(Node* left, Node* right);
	Node* opBitOr(Node* left, Node* right);
	Node* opBitXor(Node* left, Node* right);
	
	Node* opBitAndCT(Node* left, Node* right, ZClass* e);
	Node* opBitOrCT(Node* left, Node* right, ZClass* e);
	Node* opBitXorCT(Node* left, Node* right, ZClass* e);
	
	Node* opLog(Node* left, Node* right, OpNode::Type op);
	Node* opLogCT(Node* left, Node* right, OpNode::Type op);
	
	CallNode* call(Overload& over);
	RetNode* ret(Node* value = nullptr);
	
	bool FoldConstants = false;

private:
	Assembly& ass;
	
	NodePool<ConstNode> constNodes;
	NodePool<VarNode> varNodes;
	NodePool<MemNode> memNodes;
	NodePool<BlockNode> blockNodes;
	NodePool<OpNode> opNodes;
	NodePool<CastNode> castNodes;
	NodePool<CallNode> callNodes;
	NodePool<RetNode> retNodes;
	NodePool<AssignNode> assNodes;
};

}

#endif
