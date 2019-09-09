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
	RunTest("00-basic\\01-small-ct");
	RunTest("00-basic\\02-small-ct-error");
	RunTest("00-basic\\03-short-ct");
	RunTest("00-basic\\04-short-ct-error");
}

CONSOLE_APP_MAIN {
	RunMicroTests();
	
	Assembly ass;
	Compiler compiler(ass);
	
	compiler.CompileSnip("   5;  /*  \n^  */ 6; 1.1; - 1; -/*** */5;");
	Cout() << compiler.GetResult();
}
