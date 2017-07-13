#ifndef _z2c_ScannerP1_h_
#define _z2c_ScannerP1_h_

#include <Core/Core.h>

using namespace Upp;

#include "Source.h"
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
	bool win;

	void ScanUsing();
	void ScanClass();
	void ScanEnum();
	void ScanNamespace();
	void ScanAlias();
	void ScanVar(ZClass& cls, bool stat = false);
	void ScanProperty(ZClass& cls, bool stat = false);
	void ScanConst(ZClass& cls);
	Def& ScanDef(ZClass& cls, bool cons, bool stat = false, int virt = 0, bool cst = false);
	void ScanToken();
	void ScanBlock();
	void ClassLoop(ZClass& cls, bool cond = true);
	void EnumLoop(ZClass& cls);
	void ScanType();
	void ScanIf();
	void ScanIf(ZClass& cls);
};

#endif