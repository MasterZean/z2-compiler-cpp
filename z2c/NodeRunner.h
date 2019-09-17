#ifndef _z2c_NodeRunner_h_
#define _z2c_NodeRunner_h_

#include <Core/Core.h>

#include "Node.h"
#include "Assembly.h"
#include "IRGenerator.h"

namespace Z2 {
	
using namespace Upp;

class NodeRunner {
public:
	NodeRunner(Assembly& aAss, Stream& ss): ass(aAss), stream(ss), irg(aAss) {
		irg.FoldConstants = true;
	}
	
	void Execute(Overload& over);
	
	Node* Execute(Node* node);
	
	Node* ExecuteNode(ConstNode& node);
	Node* ExecuteNode(VarNode& node);
	Node* ExecuteNode(MemNode& node);
	Node* ExecuteNode(BlockNode& node);
	Node* ExecuteNode(OpNode& node);
	Node* ExecuteNode(CastNode& node);
	
	void WriteValue(Node* node);
	
	void SS() {
		for (int i = 0; i < indent; i++)
			stream << "\t";
	}
	
	void NL() {
		stream << "\r\n";
		line++;
	}
	
private:
	Assembly& ass;
	Stream& stream;
	IRGenerator irg;
	
	int indent = 0;
	int line = 0;
};

}

#endif
