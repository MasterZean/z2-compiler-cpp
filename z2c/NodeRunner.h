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
	
	Node* ExecuteOverload(Overload& over);
	
	Node* Execute(Node* node);
	
	Node* ExecuteNode(ConstNode& node);
	Node* ExecuteNode(VarNode& node);
	Node* ExecuteNode(MemNode& node);
	Node* ExecuteNode(BlockNode& node);
	Node* ExecuteNode(OpNode& node);
	Node* ExecuteNode(UnaryOpNode& node);
	Node* ExecuteNode(CastNode& node);
	Node* ExecuteNode(CallNode& node);
	Node* ExecuteNode(RetNode& node);
	Node* ExecuteNode(AssignNode& node);
	Node* ExecuteNode(ListNode& node);
	
	void WriteValue(Stream& stream, Node* node);
	
	void SS() {
		for (int i = 0; i < indent; i++)
			stream << "\t";
	}
	
	void NL() {
		stream << "\r\n";
		line++;
	}
	
	int CallDepth = 0;
	int StartCallDepth = 2;
	
private:
	Assembly& ass;
	Stream& stream;
	IRGenerator irg;
	
	Vector<Node*>* paramList = nullptr;
	
	int indent = 0;
	int line = 0;
};

}

#endif
