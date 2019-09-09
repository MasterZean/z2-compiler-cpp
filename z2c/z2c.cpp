#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"

using namespace Z2;

void RunTest(const String& path) {
	String test = NativePath(GetFileDirectory(path) + GetFileTitle(path));

	String file = LoadFile(test + ".txt");
	String out = LoadFile(test + ".out");
	
	Assembly ass;
	Compiler compiler(ass);
	compiler.PrintErrors = false;
	
	compiler.CompileSnip(file);
	String result = compiler.GetResult() + compiler.GetErrors();
	
	if (result != out) {
		LOG("=============================== TEST ===============================");
		DUMP(path);
		LOG("------------------------------ RESULT ------------------------------");
		LOG(result);
		LOG("----------------------------- EXPECTED -----------------------------");
		LOG(out);
		LOG("====================================================================");
		LOG("");
		
		Cout() << path << " FAILLED!\n";
	}
}

void RunSuite(const String& suite) {
	Vector<String> files;

	FindFile ff(NativePath(suite + "/*"));
	
	while (ff) {
		if (ff.IsFile() && GetFileExt(ff.GetName()) == ".txt")
			files << ff.GetPath();
		ff.Next();
	}
	
	Sort(files);
	
	for (int i = 0; i < files.GetCount(); i++)
		RunTest(files[i]);
}

void RunMicroTests() {
	RunSuite(GetDataFile("tests/01-basic-ct"));
	RunSuite(GetDataFile("tests/02-basic-var"));
}

CONSOLE_APP_MAIN {
	if (true)
		RunMicroTests();
	
	Assembly ass;
	Compiler compiler(ass);
	
	compiler.CompileSnip(LoadFile(GetDataFile("test.txt")));
	Cout() << compiler.GetResult();
}
