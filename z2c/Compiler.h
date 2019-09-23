#ifndef _z2c2_Compiler_h_
#define _z2c2_Compiler_h_

#include <Core/Core.h>

#include "ZParser.h"
#include "Node.h"
#include "IRGenerator.h"
#include "Assembly.h"
#include "CppNodeWalker.h"
#include "ErrorReporter.h"

namespace Z2 {
	
using namespace Upp;

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
	
	ZClass* CompileSource(const String& snip);
	bool CompileSource(ZClass& conCls, ZParser& parser);
	
	void BuildSignature(ZClass& conCls, Overload& over);
	
	bool CompileOverload(Overload& overload, ZParser& parser);
	
	bool CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level);
	bool CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	Node* CompileExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	void WriteOverloadBody(CppNodeWalker& cpp, Overload& overload, int indent = 0);
	void WriteOverload(CppNodeWalker& cpp, Overload& overload);
	
	Node* CompileVar(ZClass& conCls, Overload* conOver, ZParser& parser);
	void  CheckLocalVar(ZClass& conCls, Overload* conOver, const String& varName, const Point& p);
	Node* GetVarDefault(ZClass* cls);
	
	Node* ParseExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	Node* ParseBin(ZClass& conCls, Overload* conOver, ZParser& parser, int prec, Node* left, CParser::Pos& backupPoint, bool secondOnlyAttempt = false);
	
	Node* ParseAtom(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseId(ZClass& conCls, Overload* conOver, Overload* searchOver, ZParser& parser);
	Node* ParseNumeric(ZClass& conCls, ZParser& parser);
	Node* ParseTemporary(ZClass& conCls, Overload* conOver, ZParser& parser, const Point p, ZClass& cls);
	
	String GetResult() {
		return ss.GetResult();
	}
	
	ZClass* GetClass(ZClass& conCls, const Point& p, const String& name);
	ZClass* GetClass(const String& name);
	
	bool CanAssign(ZClass* cls, Node* n);
	
	String GetErrors();
	
	void Scan(ZClass& conCls, ZParser& parser);
	void ScanToken(ZParser& parser);
	
	bool PrintErrors = true;
	
	static String GetName() {
		return "";
	}
	
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
