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
	
	Overload* CompileSnipFunc(const String& snip);
	
	ZClass* CompileAnonClass(const String& snip);
	bool CompileSourceLoop(ZClass& conCls, ZParser& parser);
	
	void BuildSignature(ZClass& conCls, Overload& over);
	void BuildSignature(ZClass& conCls, Overload& over, ZParser& parser);
	
	bool CompileOverload(Overload& overload, ZParser& parser);
	bool CompileOverloadJump(Overload& overload);
	
	bool CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level);
	bool CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser);
	
	Node* CompileExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	Node* CompileVar(ZClass& conCls, Overload* conOver, ZParser& parser, bool cst);
	void  CheckLocalVar(ZClass& conCls, Overload* conOver, const String& varName, const Point& p);
	Node* GetVarDefault(ZClass* cls);
	
	Node* ParseExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	Node* ParseBin(ZClass& conCls, Overload* conOver, ZParser& parser, int prec, Node* left, CParser::Pos& backupPoint, bool secondOnlyAttempt = false);
	
	Node* ParseAtom(ZClass& conCls, Overload* conOver, ZParser& parser);
	Node* ParseId(ZClass& conCls, Overload* conOver, Overload* searchOver, ZParser& parser);
	Node* ParseNumeric(ZClass& conCls, ZParser& parser);
	Node* ParseTemporary(ZClass& conCls, Overload* conOver, ZParser& parser, const Point p, ZClass& cls);
	Node* ParseDot(ZClass& conCls, Overload* conOver, ZParser& parser, Node* exp);
	
	String GetResult() {
		return ss.GetResult();
	}
	
	Node* AssignOp(ZClass& conCls, Overload& conDef, Point p, Node* node, Node* rs, OpNode::Type op);
	
	ZClass* GetClass(ZClass& conCls, const Point& p, const String& name);
	ZClass* GetClass(const String& name);
	
	String GetErrors();
	
	void Scan(ZClass& conCls, ZParser& parser);
	void ScanDef(ZClass& conCls, ZParser& parser, bool ct);
	void ScanToken(ZParser& parser);
	
	void FixupParams(Overload& over, Vector<Node*>& params);
	
	bool PrintErrors = true;
	
	void Sanitize(ZClass& cls);
	void Sanitize(Method& m);
	
	static String GetName() {
		return "";
	}
	
	String BuildProfile;
	String BuildPath;
	String OutPath;
	String OrigOutPath;
	
private:
	Assembly& ass;
	IRGenerator irg;
	CppNodeWalker cpp;
	StringStream ss;
	
	Vector<ZSyntaxError> errors;
	Vector<Overload*> postOverloads;
	Vector<Overload*> compileStack;
		
	int GetPriority(CParser& parser, int& op, bool& opc);
	void GetParams(Vector<Node*>& params, ZClass& cls, Overload* def, ZParser& parser, char end = ')');
	
	static Point OPS[256];
	static bool OPCONT[256];
	
	static void SetupTables();
};

}

#endif
