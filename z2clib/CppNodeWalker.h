#ifndef _z2clib_CppNodeWalker_h_
#define _z2clib_CppNodeWalker_h_

#include "NodeWalker.h"

class BaseCppNodeWalker: public NodeWalker {
public:
	BaseCppNodeWalker(Assembly& ass, Stream& s): NodeWalker(ass, s) {
	}
	
	void Walk(ConstNode& node, Stream& stream);
	
	void WriteLocalCArrayLiteralElement(Stream& cs, const ZClass& ce, Node& n, const String& name, int index = -1);
	void WriteLocalCArrayLiteral(Stream& cs, const ZClass& ce, const RawArrayNode& list, const String& name, int count);
	
	void WriteClassName(const ZClass& ce) {
		if (!ce.CoreSimple)
			*cs << "::";
		*cs << ce.BackendName;
	}
	
protected:
	size_t rawIndex = -1;
	size_t rawSize = -1;
};

#endif
