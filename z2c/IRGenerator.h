#ifndef _z2c2_IRGenerator_h_
#define _z2c2_IRGenerator_h_

#include <Core/Core.h>

using namespace Upp;

#include "Node.h"

namespace Z2 {
	
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

	ConstNode* constIntSigned(int64 l, int base = 10, ZClass* cls = nullptr);
	ConstNode* constIntUnsigned(uint64 l, int base = 10, ZClass* cls = nullptr);
	ConstNode* constFloatSingle(double l);
	ConstNode* constFloatDouble(double l);
	
	VarNode* defineLocalVar(Variable& v);
	MemNode* mem(Variable& v);

private:
	Assembly& ass;
	
	NodePool<ConstNode> constNodes;
	NodePool<VarNode> varNodes;
	NodePool<MemNode> memNodes;
};

}

#endif
