#ifndef _z2c_NodeWalker_h_
#define _z2c_NodeWalker_h_

#include "Node.h"
#include "Assembly.h"

class Compiler;

class NodeWalker {
public:
	int indent = 0;
	Stream* cs;
	bool ClassConsts = false;
	Overload* Over = nullptr;
	Compiler* Comp = nullptr;
	String ForEachName = "_forIndex";

	NodeWalker(Assembly& assembly, Stream& s): ass(assembly), cs(&s) {
	}

	void NL() {
		for (int i = 0; i < indent; i++)
			*cs << '\t';
	}

	void EL() {
		*cs << '\n';
	}

	void ES() {
		*cs << ";\n";
	}

	virtual bool Traverse() {
		return false;
	}

	void Walk(Node* node, Stream& ss) {
		Stream* back = cs;
		cs = &ss;
		WalkNode(node);
		cs = back;
	}

	virtual void WalkNode(Node* node) {
	}

	virtual void WalkStatement(Node* node) {
	}
	
protected:
	Assembly& ass;
};

#endif