#include "BuildMethod.h"

String NormalizePathNN(const String& path) {
	return IsNull(path) ? path : NormalizePath(path);
}

#ifdef PLATFORM_WIN32

bool ExistProgram(String& bin, const char *dir, const char *file) {
	String win = NormalizePath(GetWindowsDirectory());
	if(FileExists(AppendFileName(win.Mid(0, 3) + dir, file))) {
		bin = win.Mid(0, 3) + dir;
		return true;
	}
	return false;
}

bool BuildMethod::DetectMSC7_1() {
	String vs = GetWinRegString("ProductDir", "SOFTWARE\\Microsoft\\VisualStudio\\7.1\\Setup\\VC");
	Compiler = NormalizePathNN(
			Nvl(
				GetWinRegString("InstallLocation",
	                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"
	                            "{362882AE-E40A-4435-B214-6420634C401F}"),
	            vs)
	    );
	
	if (TestLib(true, false)) {
		Name = "MSC7.1";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC8() {
	String sdk8 = GetWinRegString("InstallationFolder",
                             "Software\\Microsoft\\Microsoft SDKs\\Windows\\v6.0",
                             HKEY_CURRENT_USER);
	String bin8;
	if(!IsNull(sdk8)) {
		sdk8 = NormalizePath(sdk8);
		Sdk = sdk8;
		Compiler = bin8 = sdk8;
	}
	else {
		Compiler = bin8 = NormalizePathNN(
			GetWinRegString("8.0", "SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VS7")
		);
		Sdk = NormalizePathNN(GetWinRegString("Install Dir", "SOFTWARE\\Microsoft\\MicrosoftSDK\\InstalledSDKs\\8F9E5EF3-A9A5-491B-A889-C58EFFECE8B3"));
	}
	bool create = !IsNull(Compiler);
	bool create64 = false;
	if(bin8.GetLength() && FileExists(AppendFileName(bin8, "VC\\Bin\\x64\\cl.exe"))) {
		create64 = true;
	}
	
	if (TestLib(create, create64)) {
		Name = "MSC8";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC9() {
	String sdk9 = NormalizePathNN(GetWinRegString("InstallationFolder",
	                                       "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.0",
	                                       HKEY_LOCAL_MACHINE));
	if(IsNull(sdk9))
		sdk9 = NormalizePathNN(GetWinRegString("InstallationFolder",
	                                           "Software\\Microsoft\\Microsoft SDKs\\Windows\\v6.1",
	                                           HKEY_CURRENT_USER));
	if(IsNull(sdk9))
		sdk9 = NormalizePathNN(GetWinRegString("InstallationFolder",
		                                       "Software\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A",
		                                       HKEY_CURRENT_USER));
	String bin9;
	if(!IsNull(sdk9) && FileExists(AppendFileName(sdk9, "VC\\Bin\\cl.exe")))
		bin9 = sdk9;
	else
		ExistProgram(bin9, "Program Files (x86)\\Microsoft Visual Studio 9.0", "VC\\Bin\\cl.exe")
		|| ExistProgram(bin9, "Program Files\\Microsoft Visual Studio 9.0", "VC\\Bin\\cl.exe");
	Sdk = sdk9;
	Compiler = bin9;
	bool create = !IsNull(Compiler);
	bool create64 = false;
	String vc9_64 = AppendFileName(bin9, "VC\\Bin\\x64");
	if(!FileExists(AppendFileName(vc9_64, "cl.exe")))
		vc9_64 = AppendFileName(bin9, "VC\\Bin\\amd64");
	if(bin9.GetLength() && FileExists(AppendFileName(vc9_64, "cl.exe")))
		create64 = true;
	
	if (TestLib(create, create64)) {
		Name = "MSC9";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC10() {
	String sdk10 = NormalizePathNN(GetWinRegString("InstallationFolder",
	                                       "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.1",
	                                       HKEY_LOCAL_MACHINE));
	if (IsNull(sdk10))
		sdk10 = NormalizePathNN(GetWinRegString("InstallationFolder",
		                                        "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v7.0A",
		                                        HKEY_LOCAL_MACHINE));
	String bin10;
	if(!IsNull(sdk10) && FileExists(AppendFileName(sdk10, "VC\\Bin\\cl.exe")))
		bin10 = sdk10;
	else
		ExistProgram(bin10, "Program Files (x86)\\Microsoft Visual Studio 10.0", "VC\\Bin\\cl.exe")
		|| ExistProgram(bin10, "Program Files\\Microsoft Visual Studio 10.0", "VC\\Bin\\cl.exe");
	Sdk = sdk10;
	Compiler = bin10;
	bool create = !IsNull(Compiler);
	bool create64 = false;
	String vc10_64 = AppendFileName(bin10, "VC\\Bin\\x64");
	if(!FileExists(AppendFileName(vc10_64, "cl.exe")))
		vc10_64 = AppendFileName(bin10, "VC\\Bin\\amd64");
	if(bin10.GetLength() && FileExists(AppendFileName(vc10_64, "cl.exe")))
		create64 = true;
	
	if (TestLib(create, create64)) {
		Name = "MSC10";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC11() {
	String sdk11 = NormalizePathNN(GetWinRegString("InstallationFolder",
	                                       "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v8.0",
	                                       HKEY_LOCAL_MACHINE));
	String bin11;
	if(!IsNull(sdk11) && FileExists(AppendFileName(sdk11, "VC\\bin\\cl.exe")))
		bin11 = sdk11;
	else
		ExistProgram(bin11, "Program Files (x86)\\Microsoft Visual Studio 11.0", "VC\\bin\\cl.exe")
		|| ExistProgram(bin11, "Program Files\\Microsoft Visual Studio 11.0", "VC\\bin\\cl.exe");
	Sdk = sdk11;
	Compiler = bin11;
	bool create = !IsNull(Compiler);
	bool create64 = false;
	String vc11_64 = AppendFileName(bin11, "VC\\bin\\x64");
	if (!FileExists(AppendFileName(vc11_64, "cl.exe")))
		vc11_64 = AppendFileName(bin11, "VC\\bin\\x86_amd64");
	if (bin11.GetLength() && FileExists(AppendFileName(vc11_64, "cl.exe")))
		create64 = true;
	
	if (TestLib(create, create64)) {
		Name = "MSC11";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC12() {
	String sdk12 = NormalizePathNN(GetWinRegString("InstallationFolder",
	                                       "SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v8.1",
	                                       HKEY_LOCAL_MACHINE));
	String bin12;
	if(!IsNull(sdk12) && FileExists(AppendFileName(sdk12, "VC\\bin\\cl.exe")))
		bin12 = sdk12;
	else
		ExistProgram(bin12, "Program Files (x86)\\Microsoft Visual Studio 12.0", "VC\\bin\\cl.exe")
		|| ExistProgram(bin12, "Program Files\\Microsoft Visual Studio 12.0", "VC\\bin\\cl.exe");
	Sdk = sdk12;
	Compiler = bin12;
	bool create = !IsNull(Compiler);
	bool create64 = false;
	String vc12_64 = AppendFileName(bin12, "VC\\bin\\x64");
	if(!FileExists(AppendFileName(vc12_64, "cl.exe")))
		vc12_64 = AppendFileName(bin12, "VC\\bin\\x86_amd64");
	if(bin12.GetLength() && FileExists(AppendFileName(vc12_64, "cl.exe")))
		create64 = true;
	
	if (TestLib(create, create64)) {
		Name = "MSC12";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

bool BuildMethod::DetectMSC14() {
	String sdk12 = NormalizePathNN(GetWinRegString("KitsRoot10",
	                                       "SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots",
	                                       HKEY_LOCAL_MACHINE));
	String bin12;
	if(!IsNull(sdk12) && FileExists(AppendFileName(sdk12, "VC\\bin\\cl.exe")))
		bin12 = sdk12;
	else
		ExistProgram(bin12, "Program Files (x86)\\Microsoft Visual Studio 14.0", "VC\\bin\\cl.exe")
		|| ExistProgram(bin12, "Program Files\\Microsoft Visual Studio 14.0", "VC\\bin\\cl.exe");
	Sdk = sdk12;
	Compiler = bin12;
	bool create = !IsNull(Compiler);
	bool create64 = false;
	String vc12_64 = AppendFileName(bin12, "VC\\bin\\x64");
	if(!FileExists(AppendFileName(vc12_64, "cl.exe")))
		vc12_64 = AppendFileName(bin12, "VC\\bin\\x86_amd64");
	if(bin12.GetLength() && FileExists(AppendFileName(vc12_64, "cl.exe")))
		create64 = true;
	
	if (TestLib(create, create64)) {
		Name = "MSC14";
		Type = BuildMethod::btMSC;
		return true;
	}
	else
		return false;
}

#endif

void BuildMethod::Get(Vector<BuildMethod>& methods) {
	methods.Clear();
	
#ifdef PLATFORM_WIN32
	FindFile ff(GetCurrentDirectory() + "\\gcc\\*");
	
	while (ff) {
		if (ff.IsDirectory()) {
			String s = ff.GetName();
			if (s != ".." && s != ".") {
				String p = ff.GetPath();
				if (FileExists(p + "\\bin\\c++.exe")) {
					BuildMethod gcc;
					gcc.Name = s;
					gcc.Compiler = p;
					gcc.Sdk = p;
					
					if (FindFile(gcc.Sdk + "\\lib\\*.a")) {
						gcc.Lib32 << gcc.Sdk + "\\lib\\";
						gcc.Lib64 << gcc.Sdk + "\\lib\\";
						gcc.Type = BuildMethod::btGCC;
						methods.Add(gcc);
					}
				}
			}
		}
		
		ff.Next();
	}

	BuildMethod msc14;
	if (msc14.DetectMSC14())
		methods.Add(msc14);
	BuildMethod msc12;
	if (msc12.DetectMSC12())
		methods.Add(msc12);
	BuildMethod msc11;
	if (msc11.DetectMSC11())
		methods.Add(msc11);
	BuildMethod msc10;
	if (msc10.DetectMSC10())
		methods.Add(msc10);
	BuildMethod msc9;
	if (msc9.DetectMSC9())
		methods.Add(msc9);
	BuildMethod msc8;
	if (msc8.DetectMSC8())
		methods.Add(msc8);
	BuildMethod msc7_1;
	if (msc7_1.DetectMSC7_1())
		methods.Add(msc7_1);
#endif

#ifdef PLATFORM_POSIX
	String t, tt;
	LocalProcess lp("which c++");
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	bool res = lp.GetExitCode() == 0;
	
	String cppExe = TrimBoth(tt);
	
	if (res && cppExe.GetLength()) {
		String p = GetTempPath();
		String pc = p + "/A42.cpp";
		String po = p + "/A42";
		 
		{
			FileOut f(pc);
			f << "#include <iostream>\n int main() {\n\tstd::cout << \"A42\";\n\treturn 0;\n }\n";
		}
		
		BuildMethod gcc;
		gcc.Name = "GCC";
		gcc.Type = btGCC;
		gcc.Compiler = cppExe;
		
		{
			LocalProcess lp(cppExe + " " + pc + " -m32 -o " + po);
			tt = "";
			while (lp.Read(t)) {
				if (t.GetCount())
					tt << t;
			}
			res = lp.GetExitCode() == 0;
			
			if (res)
				gcc.Lib32.Add("");
		}
		
		
		{
			LocalProcess lp(cppExe + " " + pc + " -m64 -o " + po);
			tt = "";
			while (lp.Read(t)) {
				if (t.GetCount())
					tt << t;
			}
			res = lp.GetExitCode() == 0;
			
			if (res)
				gcc.Lib64.Add("");
		}
		
		if (!gcc.Lib32.IsEmpty() || !gcc.Lib64.IsEmpty())
			methods.Add(gcc);
	}
#endif

	for (int i = 0; i < methods.GetCount(); i++) {
		BuildMethod& m = methods[i];
		Cout() << m.Name << " Compiler Path: " << m.Compiler << "\n";
		Cout() << m.Name << " SDK Path: " << m.Sdk << "\n";
	}
}


bool BuildMethod::TestLib(bool px86, bool px64) {
	if (Compiler.IsEmpty() || Sdk.IsEmpty())
		return false;
	
	if (px86) {
		Vector<String> x86um;
		Vector<String> x86ucrt;
		
		if (FindFile(Sdk + "\\lib\\*.lib"))
			x86um.Add("\\lib\\");
		else if (FindFile(Sdk + "\\lib\\x86\\*.lib"))
			x86um.Add("\\lib\\x86\\");
		else if (FindFile(Sdk + "\\lib\\win8\\um\\x86\\*.lib"))
			x86um.Add("\\lib\\win8\\um\\x86\\");
		else if (FindFile(Sdk + "\\lib\\winv6.3\\um\\x86"))
			x86um.Add("\\lib\\winv6.3\\um\\x86\\");
		else {
			FindFile ff(Sdk + "\\lib\\*");
			
			while (ff) {
				if (ff.IsDirectory()) {
					String s = ff.GetName();
					if (s != ".." && s != ".") {
						String p = ff.GetPath();
						if (FileExists(p + "\\um\\x86\\User32.Lib")) {
							x86um.Add("\\lib\\" + ff.GetName() + "\\um\\x86\\");
							//break;
						}
						if (FileExists(p + "\\ucrt\\x86\\*.lib"))
							x86ucrt.Add("\\lib\\" + ff.GetName() + "\\ucrt\\x86\\");
					}
				}
				
				ff.Next();
			}
		}
		
		if (x86um.GetCount() == 0)
			return false;
		for (int i = x86um.GetCount() - 1; i >= 0; i--) {
			Lib32 << Sdk + x86um[i];
		}
		for (int i = x86ucrt.GetCount() - 1; i >= 0; i--) {
			Lib32 << Sdk + x86ucrt[i];
		}
		if (FileExists(Compiler + "\\VC\\lib\\LIBCMT.lib"))
			Lib32 << Compiler + "\\VC\\lib\\";
		else
			Lib32.Clear();
	}
	
	
	if (px64) {
		Vector<String> x86um;
		Vector<String> x86ucrt;
		
		if (FindFile(Sdk + "\\lib\\*.lib"))
			x86um.Add("\\lib\\");
		else if (FindFile(Sdk + "\\lib\\x64\\*.lib"))
			x86um.Add("\\lib\\x64\\");
		else if (FindFile(Sdk + "\\lib\\win8\\um\\x64\\*.lib"))
			x86um.Add("\\lib\\win8\\um\\x64\\");
		else if (FindFile(Sdk + "\\lib\\winv6.3\\um\\x64"))
			x86um.Add("\\lib\\winv6.3\\um\\x64\\");
		else {
			FindFile ff(Sdk + "\\lib\\*");
			
			while (ff) {
				if (ff.IsDirectory()) {
					String s = ff.GetName();
					if (s != ".." && s != ".") {
						String p = ff.GetPath();
						if (FileExists(p + "\\um\\x64\\User32.Lib"))
							x86um.Add("\\lib\\" + ff.GetName() + "\\um\\x64\\");
						if (FileExists(p + "\\ucrt\\x64\\*.lib"))
							x86ucrt.Add("\\lib\\" + ff.GetName() + "\\ucrt\\x64\\");
					}
				}
				
				ff.Next();
			}
		}
		
		if (x86um.GetCount() == 0)
			return false;
		for (int i = x86um.GetCount() - 1; i >= 0; i--) {
			Lib64 << Sdk + x86um[i];
		}
		for (int i = x86ucrt.GetCount() - 1; i >= 0; i--) {
			Lib64 << Sdk + x86ucrt[i];
		}
		
		if (FileExists(Compiler + "\\VC\\lib\\amd64\\LIBCMT.lib"))
			Lib64 << Compiler + "\\VC\\lib\\amd64\\";
		else if (FileExists(Compiler + "\\VC\\lib\\LIBCMT.lib"))
			Lib64 << Compiler + "\\VC\\lib\\";
		else
			Lib64.Clear();
	}
	
	return !Lib32.IsEmpty() || !Lib64.IsEmpty();
}