#include <Core/Core.h>

using namespace Upp;

#include "Compiler.h"
#include "StopWatch.h"
#include "NodeRunner.h"
#include "CommandLine.h"
#include "BuildMethod.h"
#include "Builder.h"

using namespace Z2;

int attemptedTests = 0;
int failledTests = 0;
Index<String> ext;

void RunTest(const String& path) {
	attemptedTests++;
	
	String test = NativePath(GetFileDirectory(path) + GetFileTitle(path));

	String file;
	
	bool snip = false;
	if (FileExists(test + ".in"))
		file = LoadFileBOM(test + ".in");
	else if (FileExists(test + ".snip")) {
		snip = true;
		file = LoadFileBOM(test + ".snip");
	}
	
	String out = LoadFileBOM(test + ".out");
	String out2 = String::GetVoid();
	
	if (FileExists(test + ".outemu"))
		out2 = LoadFileBOM(test + ".outemu");
	
	Assembly ass;
	Compiler compiler(ass);
	compiler.PrintErrors = false;
	
	StringStream ss;
	
	CppNodeWalker cpp(ass, ss);
	cpp.PrintDupeErrors = false;
	
	Overload* over = nullptr;
	
	if (!snip) {
		over = compiler.CompileSnipFunc(file);
		cpp.WriteOverloadBody(*over);
	}
	else {
		ZSource src;
		src.Data = file;
		
		ZClass* cls = compiler.CompileModule(src);
		compiler.Sanitize(*cls);
	
		cpp.CompilationUnitIndex = 1;
		cpp.WriteClassVars(*cls);
		
		for (int j = 0; j < cls->Methods.GetCount(); j++) {
			Method& m = cls->Methods[j];
			cpp.WriteMethod(m);
		}
		
		int i = cls->Methods.Find("@main");
	
		if (i != -1)
			over = &cls->Methods[i].Overloads[0];
	}
	
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

		Cout() << path << " FAILLED CODE GENERATION!\n";
	}
	
	if (over && !out2.IsVoid()) {
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
	
			//SaveFile("c:\\temp\\a.txt", result);
			//SaveFile("c:\\temp\\b.txt", out2);
		
			Cout() << path << " FAILLED EMULATION!\n";
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
		
		if (ff.IsFile() && (GetFileExt(ff.GetName()) == ".in" || GetFileExt(ff.GetName()) == ".snip"))
			files << ff.GetPath();
		ff.Next();
	}
	
	Sort(files);
	
	for (int i = 0; i < files.GetCount(); i++)
		RunTest(files[i]);
}

void RunMicroTests() {
	StopWatch sw;

	//RunTest("c:/Dev/z2c-reboot/z2c/tests/20-call/01-overloading/01-small/03-sc-m.snip");
	
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
	RunSuite(GetDataFile("tests/13-eq/02-small"));
	RunSuite(GetDataFile("tests/14-neq/01-bool"));
	
	RunSuite(GetDataFile("tests/20-call/01-overloading/01-small"));
	RunSuite(GetDataFile("tests/20-call/01-overloading/02-small"));
	
	RunSuite(GetDataFile("tests/21-if"));
	
	DUMP(ext);
		
	if (attemptedTests > 0 && failledTests != 0)
		Cout() << IntStr(failledTests) << " out of " << IntStr(attemptedTests) << " tests FAILED!\r\n";
	else if (attemptedTests > 0) {
		LOG(IntStr(attemptedTests) << " tests passed successfully in " + sw.ToString() + " seconds.\r\n");
		Cout() << attemptedTests << " tests passed successfully in " << sw.ToString() << " seconds.\r\n";
	}
}

CONSOLE_APP_MAIN {
	//RunMicroTests();
	
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
	
	Vector<BuildMethod> methods;
	
	String curDir = GetCurrentDirectory() + "/";

	// temporary hack pointing to a random GCC from disk
	String exeDir = "c:\\z2c\\";
	SetCurrentDirectory(exeDir);
	
	String bmPath = GetDataFile("buildMethods.xml");
	
	// load existing BMs
	LoadFromXMLFile(methods, bmPath);
	
	if (methods.GetCount() == 0) {
		methods.Clear();
		
		Cout() << "No cached build method found! Trying to auto-detect...\n";
		BuildMethod::Get(methods);
		
		if (methods.GetCount() == 0) {
			Cout() << Compiler::GetName() << " couldn't find a backend compiler. Skiping compilation step..." << '\n';
			K.SCU = false;
		}
		
		StoreAsXMLFile(methods, "methods", bmPath);
	}
	
	if (!K.BM) {
		Cout() << Compiler::GetName() << " requires a build method specified (-bm). Exiting!" << '\n';
		SetExitCode(-1);
		return;
	}
	
	int bmi = -1;
	
	// find input BM
	for (int i = 0; i < methods.GetCount(); i++)
		if (ToUpper(K.BMName) == ToUpper(methods[i].Name)) {
			bmi = i;
			break;
		}
	
	if (bmi == -1) {
		Cout() << "Build method '" << ToUpper(K.BMName) << "' can't be found. Exiting!" << '\n';
		SetExitCode(-1);
		return;
	}
	
	bmi = -1;
	// find input BM with given arch
	for (int i = 0; i < methods.GetCount(); i++)
		if (ToUpper(K.BMName) == ToUpper(methods[i].Name) && ToUpper(K.ARCH) == ToUpper(methods[i].Arch)) {
			bmi = i;
			break;
		}
	
	if (bmi == -1) {
		Cout() << "Build method '" << ToUpper(K.BMName) << "' doesn't support architecture 'x86'. Exiting!" << '\n';
		SetExitCode(-1);
		return;
	}
	
	// compile
	BuildMethod& bm = methods[bmi];

	Assembly ass;
	Compiler compiler(ass);
	
#ifdef PLATFORM_WIN32
	String platform = "WIN32";
	String platformLib = "microsoft.windows";
#endif
	
#ifdef PLATFORM_POSIX
	String platform = "POSIX";
	String platformLib = "ieee.posix";
#endif

	if (IsFullPath(K.OutPath))
		compiler.OutPath = K.OutPath;
	else
		compiler.OutPath = curDir + K.OutPath;
	compiler.OrigOutPath = compiler.OutPath;

	compiler.BuildProfile = platform + ToUpper(K.ARCH) + "." + ToUpper(bm.Name) + K.O;
	compiler.BuildPath = exeDir + NativePath("build\\") + platform + "." + ToUpper(K.ARCH) + "." + ToUpper(bm.Name);
	compiler.BMName = bm.Name;
	compiler.MSC = bm.Type == BuildMethod::btMSC;
	
	RealizeDirectory(compiler.BuildPath);
	
	//compiler.AddPackage(GetDataFile("st"));
	
	ZSource src;
	src.Data = LoadFile(K.Path);
		
	ZClass* cls = compiler.CompileModule(src);
	compiler.Sanitize(*cls);
	
	FileOut ss(K.OutPath);
	
	ss << LoadFile(AppendFileName(exeDir, "codegen\\cppcode2.h"));
		
	CppNodeWalker cpp(ass, ss);
	
	if (K.BE == "c++") {
		cpp.CPP = true;
		cpp.WriteClass(*ass.CCls);
	}
	else {
		cpp.CPP = false;
	}
	
	cpp.CompilationUnitIndex = 1;
	cpp.WriteClassVars(*cls);
	
	for (int j = 0; j < cls->Methods.GetCount(); j++) {
		Method& m = cls->Methods[j];
		cpp.WriteMethod(m);
	}
	
	ss << "int main(int argc, char* argv[]) {\n  _main();\n  return 0;\n}\n";
	ss.Close();
	
	Cout() << "==========================================================================\r\n";
	
	int i = cls->Methods.Find("@main");
	
	if (i != -1) {
		StringStream ss;
		NodeRunner exe(ass, ss);
		
		exe.ExecuteOverload(cls->Methods[i].Overloads[0]);
				
		String result = ss.GetResult();
				
		Cout() << ss.GetResult();
	}
	
	StopWatch sw;

	Builder builder(bm);
	builder.ZCompilerPath(exeDir);
	builder.TargetRoot(compiler.BuildPath);
	builder.Arch(K.ARCH);
	builder.Optimize(K.O);
	builder.CPP(cpp.CPP);
	
	if (!builder.Build(compiler.OutPath, compiler.OrigOutPath))
		SetExitCode(-1);
	Cout() << bm.Name << " code generation finished in " << sw.ToString() << " seconds.\n";
}
