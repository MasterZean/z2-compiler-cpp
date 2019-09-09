#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"

using namespace Z2;

void RunTest(const String& path) {
	String file = LoadFile(GetDataFile("tests\\" + path + ".txt"));
	String out = LoadFile(GetDataFile("tests\\" + path + ".out"));
	
	Assembly ass;
	Compiler compiler(ass);
	compiler.PrintErrors = false;
	
	compiler.CompileSnip(file);
	String result = compiler.GetResult() + compiler.GetErrors();
	
	if (result != out) {
		LOG("===========================================================");
		DUMP(path);
		LOG("-----------------------------------------------------------");
		LOG(result);
		LOG("-----------------------------------------------------------");
		LOG(out);
		RealizeDirectory(GetDataFile("out\\" + GetFileFolder(path)));
		SaveFile(GetDataFile("out\\" + path + ".a"), result);
		SaveFile(GetDataFile("out\\" + path + ".b"), out);
		Cout() << path << " FAILLED!\n";
	}
}

void RunMicroTests() {
	RunTest("01-basic-ct\\01-small-ct");
	RunTest("01-basic-ct\\02-small-ct-error");
	RunTest("01-basic-ct\\03-short-ct");
	RunTest("01-basic-ct\\04-short-ct-error");
	RunTest("01-basic-ct\\05-int-ct");
	RunTest("01-basic-ct\\06-int-ct-error");
	RunTest("01-basic-ct\\07-int-ct");
	RunTest("01-basic-ct\\08-int-ct-error");
	RunTest("01-basic-ct\\09-int-ct");
	RunTest("01-basic-ct\\10-int-ct-error");
	
	RunTest("02-basic-var\\01-small-var");
}

CONSOLE_APP_MAIN {
	if (true)
		RunMicroTests();
	
	Assembly ass;
	Compiler compiler(ass);
	
	compiler.CompileSnip("val a = 7000");
	Cout() << compiler.GetResult();
}
