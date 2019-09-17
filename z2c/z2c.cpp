#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"
#include "StopWatch.h"
#include "NodeRunner.h"

using namespace Z2;

int attemptedTests = 0;
int failledTests = 0;

void RunTest(const String& path) {
	attemptedTests++;
	
	String test = NativePath(GetFileDirectory(path) + GetFileTitle(path));

	String file = LoadFileBOM(test + ".txt");
	String out = LoadFileBOM(test + ".out");
	String out2 = String::GetVoid();
	
	if (FileExists(test + ".exeout"))
		out2 = LoadFileBOM(test + ".exeout");
	
	Assembly ass;
	Compiler compiler(ass);
	compiler.PrintErrors = false;
	
	Overload* over = compiler.CompileSnip(file);
	
	StringStream ss;
	CppNodeWalker cpp(ass, ss);
	
	compiler.WriteOverloadBody(cpp, *over);
	
	String result = ss.GetResult();
	result << compiler.GetErrors();
	
	bool failled = false;
	
	if (result != out) {
		failled = true;
		
		LOG("========================= CODE GENERATION ==========================");
		DUMP(path);
		LOG("------------------------------ RESULT ------------------------------");
		LOG(result);
		LOG("----------------------------- EXPECTED -----------------------------");
		LOG(out);
		LOG("====================================================================");
		LOG("");

		Cout() << path << " FAILLED!\n";
	}
	
	if (!out2.IsVoid()) {
		StringStream ss;
		NodeRunner exe(ass, ss);
		
		exe.Execute(*over);
		
		String result = ss.GetResult();
		
		if (result != out2) {
			failled = true;
			
			LOG("============================ EXECUTION =============================");
			DUMP(path);
			LOG("------------------------------ RESULT ------------------------------");
			LOG(result);
			LOG("----------------------------- EXPECTED -----------------------------");
			LOG(out2);
			LOG("====================================================================");
			LOG("");
	
			Cout() << path << " FAILLED!\n";
		}
	}
	
	if (failled)
		failledTests++;
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
	StopWatch sw;

	RunSuite(GetDataFile("tests/01-basic-ct"));
	RunSuite(GetDataFile("tests/02-basic-var"));
	RunSuite(GetDataFile("tests/03-explicit-var"));
	RunSuite(GetDataFile("tests/04-block"));
	
	if (attemptedTests > 0 && failledTests != 0)
		Cout() << IntStr(failledTests) << " out of " << IntStr(attemptedTests) << " tests FAILED!\r\n";
	else if (attemptedTests > 0) {
		LOG(IntStr(attemptedTests) << " tests passed successfully in " + sw.ToString() + " seconds.\r\n");
		Cout() << attemptedTests << " tests passed successfully in " << sw.ToString() << " seconds.\r\n";
	}
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
		
		StringStream ss2;
		NodeRunner exe(ass, ss2);
		
		exe.Execute(over->OwnerClass.Overloads[i]);
		
		String result = ss2.GetResult();
		
		Cout() << ss2.GetResult();
	}
}

