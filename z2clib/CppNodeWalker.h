#ifndef _z2clib_CppNodeWalker_h_
#define _z2clib_CppNodeWalker_h_

#include "NodeWalker.h"

class BaseCppNodeWalker: public NodeWalker {
public:
	bool WriteCtQual = false;
	bool EqOperator = true;
	
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
	
	void WriteOverloadDefinition(Stream& cs, Overload &over);
	void WriteOverloadDeclaration(Stream& cs, Overload &over);
	void WriteOverloadNameParams(Stream& cs, Overload &over);
	void WriteOverloadVoidingList(Stream& cs, Overload &over);
	bool WriteReturnType(Stream& cs, Overload &over);
	void WriteParams(Stream& cs, Overload& over, ZClass& cls, bool ths);
	
protected:
	size_t rawIndex = -1;
	size_t rawSize = -1;
};

#endif
