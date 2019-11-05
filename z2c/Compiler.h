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

class ZPackage;

class ZFile: Moveable<ZFile> {
public:
	String Path;
	Time Modified;
	String Data;
	
	void Serialize(Stream& s) {
		s % Path % Modified;
	}
};

class ZSource: public ZFile, Moveable<ZSource> {
public:
	bool IsScaned = false;
	ZPackage* Package = nullptr;
	
	String Namespace;
	SubModule Module;
};

class ZPackage: Moveable<ZPackage> {
public:
	String Name;
	String Path;
	String CachePath;
	
	ArrayMap<String, ZSource> Files;
	
	void Serialize(Stream& s) {
		s % Name % Path % Files;
	}
};

class Scanner {
public:
	enum PlatformType {
		WINDOWS32,
		POSIX,
	};
	
	Scanner(Assembly& aAss): ass(aAss) {
	}
	
	void Scan(ZSource& src);
	
private:
	Assembly& ass;
};

class Compiler {
public:
	enum PlatformType {
		WINDOWS32,
		POSIX,
	};
	
	Compiler(Assembly& aAss): ass(aAss), irg(aAss), cpp(aAss, ss) {
		static bool tableSetup = false;
		
		if (!tableSetup) {
			SetupTables();
			tableSetup = true;
		}
		
#ifdef PLATFORM_WIN32
		Platform = WINDOWS32;
#endif

#ifdef PLATFORM_POSIX
		Platform = POSIX;
#endif
	}
	
	Overload* CompileSnipFunc(const String& snip);
	
	ZClass* CompileAnonClass(const String& snip);
	bool CompileSourceLoop(ZClass& conCls, ZParser& parser);
	
	void BuildSignature(ZClass& conCls, Overload& over);
	void BuildSignature(ZClass& conCls, Overload& over, ZParser& parser);
	
	bool CompileOverload(Overload& overload, ZParser& parser);
	bool CompileOverloadJump(Overload& overload);
	
	bool CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, Vector<Node*>* nodePool, int level);
	bool CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser, Vector<Node*>* nodePool);
	
	Node* CompileExpression(ZClass& conCls, Overload* conOver, ZParser& parser);
	
	void  CompileClass(ZClass& cls);
	Node* CompileVar(ZClass& conCls, Overload* conOver, ZParser& parser, bool cst);
	void  CompileVar(Variable& v);
	void  CheckLocalVar(ZClass& conCls, Overload* conOver, const String& varName, const Point& p);
	Node* GetVarDefault(ZClass* cls);
	
	Node* CompileIfWhile(ZClass& conCls, Overload* conOver, ZParser& parser, Vector<Node*>* nodePool, bool isIf);
	
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
	
	void Scan(ZSource& src);
	void Scan(ZClass& conCls, ZParser& parser);
	void ScanClass(ZClass& conCls, ZParser& parser);
	void ScanDef(ZClass& conCls, ZParser& parser, bool ct);
	void ScanToken(ZParser& parser);
	
	void FixupParams(Overload& over, Vector<Node*>& params);
	
	bool PrintErrors = true;
	
	void Sanitize(ZClass& cls);
	void Sanitize(Method& m);
	
	bool SkipUntilNL(ZParser& parser, bool cb = false);
	
	bool AddPackage(const String& aPath);
	void AddModule(int parent, const String& path, ZPackage& pak, ZPackage& temp);
	void AddModuleSource(ZPackage& pak, ZPackage& temp, FindFile& ff);
	ZSource& AddSource(ZPackage& aPackage, const String& aFile, bool populate = true);
	ZSource& LoadSource(ZSource& source, bool populate);
	void AddBindsFile(const String& path);
	
	static String GetName() {
		return "";
	}
	
	String BuildProfile;
	String BuildPath;
	String OutPath;
	String OrigOutPath;
	
	bool MSC = false;
	String BMName;
	PlatformType Platform;
	
private:
	Assembly& ass;
	IRGenerator irg;
	CppNodeWalker cpp;
	StringStream ss;
	int filesOpened = 0;
	
	Vector<ZSyntaxError> errors;
	Vector<Overload*> postOverloads;
	Vector<Overload*> compileStack;
	
	ArrayMap<String, ZPackage> packages;
	VectorMap<String, String> Binds;
	VectorMap<String, String>* Cache = nullptr;
		
	int GetPriority(CParser& parser, int& op, bool& opc);
	void GetParams(Vector<Node*>& params, ZClass& cls, Overload* def, ZParser& parser, char end = ')');
	
	static Point OPS[256];
	static bool OPCONT[256];
	
	static void SetupTables();
};

}

#endif
