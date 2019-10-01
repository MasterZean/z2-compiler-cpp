#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"
#include "StopWatch.h"
#include "NodeRunner.h"
#include "CommandLine.h"

using namespace Z2;

int attemptedTests = 0;
int failledTests = 0;
Index<String> ext;

void RunTest(const String& path) {
	attemptedTests++;
	
	String test = NativePath(GetFileDirectory(path) + GetFileTitle(path));

	String file = LoadFileBOM(test + ".in");
	String out = LoadFileBOM(test + ".out");
	String out2 = String::GetVoid();
	
	if (FileExists(test + ".outemu"))
		out2 = LoadFileBOM(test + ".outemu");
	
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
		LOG(test + ".out");
		LOG("------------------------------ RESULT ------------------------------");
		LOG(result);
		LOG("----------------------------- EXPECTED -----------------------------");
		LOG(out);
		LOG("====================================================================");
		LOG("");

		//SaveFile("c:\\temp\\a.txt", result);
		//SaveFile("c:\\temp\\b.txt", out);

		Cout() << path << " FAILLED!\n";
	}
	
	if (!out2.IsVoid()) {
		StringStream ss;
		NodeRunner exe(ass, ss);
		
		exe.ExecuteOverload(*over);
		
		String result = ss.GetResult();
		
		if (result != out2) {
			failled = true;
			
			LOG("============================ EXECUTION =============================");
			LOG(test + ".outemu");
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
		if (ff.IsFile())
			ext.FindAdd(GetFileExt(ff.GetName()));
		
		if (ff.IsFile() && GetFileExt(ff.GetName()) == ".in")
			files << ff.GetPath();
		ff.Next();
	}
	
	Sort(files);
	
	for (int i = 0; i < files.GetCount(); i++)
		RunTest(files[i]);
}

void RunMicroTests() {
	StopWatch sw;

	//RunTest("");
	
	RunSuite(GetDataFile("tests/01-const/01-small"));
	RunSuite(GetDataFile("tests/01-const/03-short"));
	RunSuite(GetDataFile("tests/01-basic-ct"));
	RunSuite(GetDataFile("tests/02-basic-var"));
	RunSuite(GetDataFile("tests/03-explicit-var"));
	RunSuite(GetDataFile("tests/04-block"));
	RunSuite(GetDataFile("tests/10-var-op"));
	
	RunSuite(GetDataFile("tests/11-shl/01-bool"));
	RunSuite(GetDataFile("tests/11-shl/02-small"));
	RunSuite(GetDataFile("tests/11-shl/03-byte"));
	RunSuite(GetDataFile("tests/11-shl/04-short"));
	RunSuite(GetDataFile("tests/11-shl/05-word"));
	RunSuite(GetDataFile("tests/11-shl/06-int"));
	RunSuite(GetDataFile("tests/11-shl/07-dword"));
	RunSuite(GetDataFile("tests/11-shl/08-long"));
	RunSuite(GetDataFile("tests/11-shl/09-qword"));
	
	RunSuite(GetDataFile("tests/12-shr/01-bool"));
	RunSuite(GetDataFile("tests/12-shr/02-small"));
	RunSuite(GetDataFile("tests/12-shr/03-byte"));
	RunSuite(GetDataFile("tests/12-shr/04-short"));
	RunSuite(GetDataFile("tests/12-shr/05-word"));
	RunSuite(GetDataFile("tests/12-shr/06-int"));
	RunSuite(GetDataFile("tests/12-shr/07-dword"));
	RunSuite(GetDataFile("tests/12-shr/08-long"));
	RunSuite(GetDataFile("tests/12-shr/09-qword"));
	
	RunSuite(GetDataFile("tests/13-eq/01-bool"));
	
	DUMP(ext);
		
	if (attemptedTests > 0 && failledTests != 0)
		Cout() << IntStr(failledTests) << " out of " << IntStr(attemptedTests) << " tests FAILED!\r\n";
	else if (attemptedTests > 0) {
		LOG(IntStr(attemptedTests) << " tests passed successfully in " + sw.ToString() + " seconds.\r\n");
		Cout() << attemptedTests << " tests passed successfully in " << sw.ToString() << " seconds.\r\n";
	}
}

bool IgnoreDupes = true;

CONSOLE_APP_MAIN {
	RunMicroTests();
	
	Z2::CommandLine K;
	if (!K.Read()) {
		SetExitCode(-1);
		return;
	}
	
	if (K.Path.GetCount() == 0) {
		Cout() << "No input filled specified.\n";
		SetExitCode(-1);
		return;
	}
	
	if (K.OutPath.GetCount() == 0) {
		Cout() << "No output filled specified.\n";
		SetExitCode(-1);
		return;
	}
	
	Assembly ass;
	Compiler compiler(ass);
	
	ZClass* cls = compiler.CompileSource(LoadFile(K.Path));
	
	FileOut ss(K.OutPath);
	CppNodeWalker cpp(ass, ss);
	
	cpp.WriteClassVars(*cls);
			
	const int TempCU = 1;
	
	for (int j = 0; j < cls->Methods.GetCount(); j++) {
		Method& m = cls->Methods[j];
	
		for (int i = 0; i < m.Overloads.GetCount(); i++) {
			Overload& o = m.Overloads[i];
			
			bool dupe = false;
			for (int k = 0; k < i; k++) {
				Overload& o2 = m.Overloads[k];
				
				try {
					if (o.Signature == o2.Signature) {
						dupe = true;
						ErrorReporter::Dup(cls->Name, o.NamePoint, o2.NamePoint, o.OwnerMethod.Name);
					}
				}
				catch (ZSyntaxError& err) {
					err.PrettyPrint(Cout());
				}
			}
			
			if ((IgnoreDupes && !dupe) || !IgnoreDupes) {
				int written = 0;
				
				for (int k = 0; k < o.DepOver.GetCount(); k++)
					if (o.DepOver[k]->MDecWritten != TempCU) {
						cpp.WriteOverloadDeclaration(*o.DepOver[k]);
						o.DepOver[k]->MDecWritten = TempCU;
						written++;
					}
					
				if (written)
					cpp.NL();
				
				compiler.WriteOverload(cpp, o);
			}
		}
	}
	
	//Cout() << ss.GetResult();
	
	Cout() << "==========================================================================\r\n";
	
	int i = cls->Methods.Find("@main");
	
	if (i != -1) {
		StringStream ss;
		NodeRunner exe(ass, ss);
				
		exe.ExecuteOverload(cls->Methods[i].Overloads[0]);
				
		String result = ss.GetResult();
				
		Cout() << ss.GetResult();
	}
}

