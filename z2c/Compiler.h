#ifndef _z2c2_Compiler_h_
#define _z2c2_Compiler_h_

#include <Core/Core.h>

using namespace Upp;

#include "ZParser.h"
#include "Node.h"
#include "IRGenerator.h"
#include "Assembly.h"
#include "CppNodeWalker.h"
#include "ErrorReporter.h"

namespace Z2 {

class Compiler {
public:
	Compiler(Assembly& aAss): ass(aAss), irg(aAss), cpp(aAss, ss) {
	}
	
	bool CompileSnip(const String& snip);
	
	bool CompilerOverload(Overload& overload);
	bool CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level);
	bool CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	Node* CompileVar(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	Node* Parse(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseId(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseNumeric(ZClass& conCls, ZParser& parser);
	
	String GetResult() {
		return ss.GetResult();
	}
	
	String GetErrors();
	
	bool PrintErrors = true;
	
private:
	Assembly& ass;
	IRGenerator irg;
	CppNodeWalker cpp;
	StringStream ss;
	
	Vector<ZSyntaxError> errors;
};

}

#endif
