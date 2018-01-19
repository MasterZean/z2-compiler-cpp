#ifndef _z2c_ScannerP1_h_
#define _z2c_ScannerP1_h_

#include <Core/Core.h>

using namespace Upp;

#include "source.h"
#include <z2clib/Assembly.h>

class Def;

class Scanner {
public:
	Scanner(ZSource& aSrc, bool windows): source(aSrc), win(windows) {
		parser = ZParser(aSrc.Data);
		parser.NestComments();
		parser.Mode = " scan";
		parser.Source = &aSrc;
	}

	void Scan(bool cond = true);

protected:
	ZParser parser;
	ZSource& source;
	String nameSpace;
	Entity::AccessType insertAccess;
	String bindName;
	bool win;
	bool isIntrinsic = false;
	bool isDllImport = false;
	bool isStdCall = false;
	bool isNoDoc = false;
	bool isForce = false;
	bool isCDecl = false;

	void ScanUsing();
	void ScanClass(bool foreceStatic = false);
	void ScanEnum();
	void ScanNamespace();
	void ScanAlias();
	void ScanVar(ZClass& cls, bool stat = false);
	void ScanProperty(ZClass& cls, bool stat = false);
	void ScanConst(ZClass& cls);
	Def& ScanDef(ZClass& cls, bool cons, bool stat = false, int virt = 0, bool cst = false);
	void ScanToken();
	void ScanBlock();
	void ClassLoop(ZClass& cls, bool cond = true, bool foreceStatic = false);
	void EnumLoop(ZClass& cls);
	void ScanType();
	void ScanIf();
	void ScanIf(ZClass& cls);
	
	void InterpretTrait(const String& trait);
	void ScanDefAlias(Overload& over);
	void TraitLoop();
};

#endif