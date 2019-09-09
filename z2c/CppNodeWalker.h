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
	void WalkNode(ConstNode& node);
	void WalkNode(VarNode& node);
	void WalkNode(MemNode& node);
	
	void ES() {
		stream << ";\r\n";
	}
	
private:
	Assembly& ass;
	Stream& stream;
};

}

#endif
