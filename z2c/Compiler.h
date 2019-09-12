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
		static bool tableSetup = false;
		
		if (!tableSetup) {
			SetupTables();
			tableSetup = true;
		}
	}
	
	Overload* CompileSnip(const String& snip);
	
	bool CompilerOverload(Overload& overload);
	bool CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level);
	bool CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	void WriteOverloadBody(CppNodeWalker& cpp, Overload& overload, int indent = 0);
	void WriteOverload(CppNodeWalker& cpp, Overload& overload);
	
	Node* CompileVar(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	Node* ParseExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	Node* ParseBin(ZClass& conCls, Overload* conOver, ZParser& parser, int prec, Node* left, CParser::Pos& backupPoint, bool secondOnlyAttempt = false);
	
	Node* ParseAtom(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseId(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseNumeric(ZClass& conCls, ZParser& parser);
	
	String GetResult() {
		return ss.GetResult();
	}
	
	String GetErrors();
	
	void ScanBlock(ZClass& conCls, ZParser& parser);
	void ScanToken(ZClass& conCls, ZParser& parser);
	
	bool PrintErrors = true;
	
private:
	Assembly& ass;
	IRGenerator irg;
	CppNodeWalker cpp;
	StringStream ss;
	
	Vector<ZSyntaxError> errors;
	Vector<Overload*> postOverloads;
		
	int GetPriority(CParser& parser, int& op, bool& opc);
	
	static Point OPS[256];
	static bool OPCONT[256];
	
	static void SetupTables();
};

}

#endif
