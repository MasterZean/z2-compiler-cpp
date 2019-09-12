#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"
//#include "Assembly.h"

using namespace Z2;

int attemptedTests = 0;
int failledTests = 0;

void RunTest(const String& path) {
	attemptedTests++;
	
	String test = NativePath(GetFileDirectory(path) + GetFileTitle(path));

	String file = LoadFileBOM(test + ".txt");
	String out = LoadFileBOM(test + ".out");
	
	Assembly ass;
	Compiler compiler(ass);
	compiler.PrintErrors = false;
	
	Overload* over = compiler.CompileSnip(file);
	
	StringStream ss;
	CppNodeWalker cpp(ass, ss);
	
	compiler.WriteOverloadBody(cpp, *over);
	
	String result = ss.GetResult() + compiler.GetErrors();
	
	if (result != out) {
		failledTests++;
		
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
	RunSuite(GetDataFile("tests/03-block"));
	
	if (attemptedTests > 0 && failledTests != 0)
		Cout() << IntStr(failledTests) << " out of " << IntStr(attemptedTests) << " tests FAILED!\r\n";
	else if (attemptedTests > 0)
		LOG(IntStr(attemptedTests) << " tests passed successfully.\r\n");
}

CONSOLE_APP_MAIN {
	RunMicroTests();
	
	Assembly ass;
	Compiler compiler(ass);
	
	Overload* over = compiler.CompileSnip(LoadFile(GetDataFile("test.txt")));
	
	for (int i = 0; i < over->OwnerClass.Overloads.GetCount(); i++) {
		StringStream ss;
		CppNodeWalker cpp(ass, ss);
		
		compiler.WriteOverload(cpp, over->OwnerClass.Overloads[i]);
		
		Cout() << ss.GetResult();
	}
}

