#ifndef _z2c2_CppNodeWalker_h_
#define _z2c2_CppNodeWalker_h_

#include <Core/Core.h>

using namespace Upp;

#include "Node.h"
#include "Assembly.h"

namespace Z2 {

class CppNodeWalker {
public:
	CppNodeWalker(Assembly& aAss, Stream& ss): ass(aAss), stream(ss) {
	}
	
	void Walk(Node* node);
	
	void WalkStatement(Node* node) {
		if (node->NT == NodeType::Block)
			Walk(node);
		else {
			SS();
			Walk(node);
			ES();
		}
	}
	
	void WalkNode(ConstNode& node);
	void WalkNode(VarNode& node);
	void WalkNode(MemNode& node);
	void WalkNode(BlockNode& node);
	void WalkNode(OpNode& node);
	
	void ES() {
		stream << ";\r\n";
	}
	
	void SS() {
		for (int i = 0; i < indent; i++)
			stream << "\t";
	}
	
	void ResetIndent() {
		indent = 0;
	}
	
private:
	Assembly& ass;
	Stream& stream;
	
	int indent = 0;
};

}

#endif
