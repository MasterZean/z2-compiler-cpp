#include "Builder.h"
#include "StopWatch.h"

namespace Z2 {

String QT = "\"";

bool Builder::Build(const String& path, const String& origPath) {
	if (bm.Type == BuildMethod::btMSC)
		return BuildMSC(path, origPath);
	
	if (bm.Type == BuildMethod::btGCC)
		return BuildGCC(path, origPath);

	return false;
}

#ifdef PLATFORM_WIN32

String GetWinPath() {
	wchar h[MAX_PATH];
	GetWindowsDirectoryW(h, MAX_PATH);
	return FromSystemCharsetW(h);
}

void Builder::DoPathsMSC() {
	cppPath = bm.Compiler + "\\";
	if (arch == "x64") {
		if (FindFile(cppPath + "x86_amd64\\cl.exe"))
			cppPath << "x86_amd64\\cl.exe";
		else
			cppPath << "cl.exe";
	}
	else
		cppPath << "cl.exe";
	cppPath = NormalizePath(cppPath);
	
	linkPath = bm.Compiler + "\\";
	if (arch == "x64") {
		if (FindFile(linkPath + "x86_amd64\\link.exe"))
			linkPath << "x86_amd64\\link.exe";
		else
			linkPath << "link.exe";
	}
	else
		linkPath << "link.exe";
	linkPath = NormalizePath(linkPath);
}

void Builder::DoEnvMSC() {
	StringStream ss;
	
	ss << "PATH=";
	ss << GetEnv("PATH") << ";";
	ss << bm.Tools << "\\" << ";";
	ss << bm.Compiler << "\\" << ";";
	ss.Put(0);

	ss << "LIB=";
	ss << GetEnv("LIB");
	
	for (int i = 0; i < bm.Lib.GetCount(); i++) {
		ss << bm.Lib[i] << ";";
	}
	ss.Put(0);
	
	String tempPath = GetEnv("TMP");
	ss << "TMP=";
	ss << tempPath;
	ss.Put(0);
	ss << "TEMP=";
	ss << tempPath;
	ss.Put(0);
	
	ss << "SystemRoot=" << GetWinPath();
	ss.Put(0);
	
	ss.Put(0);
	
	env = ss;
}

bool Builder::CompileMSC(const String& src, const String& out) {
	StopWatch sw;
	
	Cout() << "\tbuilding " << GetFileTitle(src) << "... ";
	String cmd;
	String t, tt;
	
	cmd << QT << cppPath << QT << " ";
	cmd << QT << src << QT << " ";
	cmd << "/Fo" << QT << out << QT << " ";
	cmd << optimize << " ";
	cmd << "/c /nologo /MT /EHsc ";
	
	DUMP(cmd);
	
	LocalProcess lp(cmd, env);
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	
	if (tt != GetFileTitle(src) + ".cpp\r\n") {
		Cout() << "FAILLED!\n";
		Cout() << tt << "\n";
		return false;
	}
	else {
		Cout() << "done in " << sw.ToString() << " seconds.\n";
		return true;
	}
}


bool Builder::BuildMSC(const String& path, const String& origPath) {
	bool result = true;

	DoPathsMSC();
	
	String inPath = GetFileDirectory(path);
	String inTitle = GetFileTitle(path);
	String outPath = GetFileDirectory(origPath);
	String outTitle = GetFileTitle(origPath);
		
	DoEnvMSC();
	DUMP(env);
	
	StopWatch sw;
	
	String d;
	String t, tt;
	
	//SetCurrentDirectory(inPath);
	
	String leakObj = AppendFileName(target, "leakdetect.obj");

	if (!FileExists(leakObj))
		if (!CompileMSC(zPath + "codegen\\leakdetect.cpp", leakObj))
			result = false;
	
	if (!CompileMSC(inPath + inTitle + ".cpp", inPath + inTitle + ".obj"))
		result = false;
	
	sw.Reset();
	Cout() << "\tlinking... ";
	d = "";
	t = "";
	tt = "";
	
	d << QT + linkPath + QT + " " + inPath + inTitle + ".obj \"" + leakObj + "\" user32.lib /nologo " +"/out:\"" + outPath + outTitle + ".exe\"";
	if (arch == "x64")
		d << " /MACHINE:x64 ";
	{
		LocalProcess lp(d, env);
		while (lp.Read(t)) {
			if (t.GetCount())
				tt << t;
		}
		if (tt.GetLength()) {
			Cout() << "FAILLED!\n";
			Cout() << tt << "\n";
			result = false;
		}
		else {
			Cout() << "done in " << sw.ToString() << " seconds.\n";
		}
	}
		
	DeleteFile(inPath + inTitle + ".obj");
	DeleteFile(inPath + inTitle + ".cpp");
	
	return result;
}

void Builder::DoEnvGCC() {
	StringStream ss;
	
	ss << "PATH=";
	//ss << bm.Tools << "\\" << ";";
	ss << bm.Compiler << "\\bin\\" << ";";
	ss << GetEnv("PATH") << ";";
	ss.Put(0);

	/*ss << "LIB=";
	ss << GetEnv("LIB");
	
	for (int i = 0; i < bm.Lib.GetCount(); i++) {
		ss << bm.Lib[i] << ";";
	}
	ss.Put(0);
	
	String tempPath = GetEnv("TMP");
	ss << "TMP=";
	ss << tempPath;
	ss.Put(0);
	ss << "TEMP=";
	ss << tempPath;
	ss.Put(0);
	
	ss << "SystemRoot=" << GetWinPath();
	ss.Put(0);*/
	
	ss.Put(0);
	
	env = ss;
}

bool Builder::CompileGCC(const String& src, const String& out) {
	StopWatch sw;
	
	Cout() << "\tbuilding " << GetFileTitle(src) << "... ";
	String cmd;
	String t, tt;
	
	/*cmd << "SET PATH=%PATH%;";
	cmd << bm.Compiler << "\\bin" << ";";
	cmd << " & ";*/
	
	cmd << QT << cppPath << QT << " ";
	cmd << "-w " << optimize << " ";
	cmd << "-c -x c++ -fpermissive -fexceptions -msse2 -std=c++11";
	cmd << " -m";
	if (arch == "x64")
		cmd << "64";
	else
		cmd << "32";
	cmd << " ";
	cmd << QT << src << QT << " ";
	cmd << "-o " << QT << out << QT << " ";
	
	DUMP(cmd);
	
	//String pp = "cmd.exe /C \"" + cmd + "\"";
	
	LocalProcess lp(cmd, env);
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	
	//DUMP(lp.GetExitMessage());
	if (!tt.IsEmpty()) {
		Cout() << "FAILLED!\n";
		Cout() << tt << "\n";
		return false;
	}
	else {
		Cout() << "done in " << sw.ToString() << " seconds.\n";
		return true;
	}
}

bool Builder::BuildGCC(const String& path, const String& origPath) {
	DUMP(path);
	String inPath = GetFileDirectory(path);
	String inTitle = GetFileTitle(path);
	String outPath = GetFileDirectory(origPath);
	String outTitle = GetFileTitle(origPath);
	String c;
	//String O = optimize;
	bool result = true;

	/*c << "SET PATH=%PATH%;";
	c << bm.Compiler << "\\bin" << ";";
	c << " & ";*/
	
	/*StringStream ss;
	
	ss << "PATH=";
	ss << GetEnv("PATH") << ";";
	ss << bm.Compiler << "\\bin" << ";";
	ss.Put(0);
	
	ss.Put(0);
	
	env = ss;
	DUMP(c);
	DUMP(env);*/
	
	cppPath = bm.Compiler + "\\bin\\c++.exe";
	linkPath = cppPath;
	
	DoEnvGCC();
	DUMP(env);
	
	if (optimize == " -Od")
		optimize = " -Og";
	
	String leakObj = AppendFileName(target, "leakdetect.o");

	if (!FileExists(leakObj))
		if (!CompileGCC(zPath + "codegen\\leakdetect.cpp", leakObj))
			result = false;
	
	if (!CompileGCC(inPath + inTitle + ".cpp", inPath + inTitle + ".o"))
		result = false;
	
	//c << "cd \"" + s + "\" & ";
	
	/*String leak = zPath + "codegen\\win32\\gcc\\leakdetect." + arch + ".o ";
	if (optimize == " -Od")
		optimize = " -Og";
	//O << " ";
	c << "\"" +  -w " + s + n + ".cpp " + leak + O + " -o " + s2 + n2 + ".exe" + " -fpermissive -fexceptions -msse2 -m";
	if (arch == "x64")
		c << "64";
	else
		c << "32";
	//c << " -g2 -static -x c++ -std=c++11 ";
	c << " -x c++ -std=c++11 ";
	c << "-luser32";
	//c << "-luser32 -lraylib -lopengl32 -lgdi32";
	String t, tt;
	String pp = "cmd.exe /C \"" + c + "\"";

	SetCurrentDirectory(s);

	LocalProcess lp(pp);
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	if (!tt.IsEmpty()) {
		Cout() << "Binary generation output: " << tt << "\n";
		result = false;
	}*/
	
	StopWatch sw;
	String d, t, tt;
	
	Cout() << "\tlinking... ";
	
	d << QT + linkPath + QT + " " + QT + inPath + inTitle + ".o" + QT + " \"" + leakObj + "\" -luser32 --static -o \"" + outPath + outTitle + ".exe\" -m";
	if (arch == "x64")
		d << "64";
	else
		d << "32";
	//d << " -Wl,--subsystem,windows";
	//if (arch == "x64")
	//	d << " /MACHINE:x64 ";
	DUMP(d);
	{
		LocalProcess lp(d, env);
		while (lp.Read(t)) {
			if (t.GetCount())
				tt << t;
		}
		if (tt.GetLength()) {
			Cout() << "FAILLED!\n";
			Cout() << tt << "\n";
			result = false;
		}
		else {
			Cout() << "done in " << sw.ToString() << " seconds.\n";
		}
	}

	DeleteFile(inPath + inTitle + ".o");
	DeleteFile(inPath + inTitle + ".cpp");
	
	return result;
}

#endif

#ifdef PLATFORM_POSIX

bool Builder::CompileMSC(const String& src, const String& out) {
	return false;
}

bool Builder::CompileGCC(const String& src, const String& out) {
	StopWatch sw;
	
	Cout() << "\tbuilding " << GetFileTitle(src) << "... ";
	String cmd;
	String t, tt;
	
	/*cmd << "SET PATH=%PATH%;";
	cmd << bm.Compiler << "\\bin" << ";";
	cmd << " & ";*/
	
	cmd << QT << cppPath << QT << " ";
	cmd << "-w " << optimize << " ";
	cmd << "-c -x c++ -fpermissive -fexceptions -msse2 -std=c++11";
	cmd << " -m";
	if (arch == "x64")
		cmd << "64";
	else
		cmd << "32";
	cmd << " ";
	cmd << QT << src << QT << " ";
	cmd << "-o " << QT << out << QT << " ";
	
	DUMP(cmd);
	//String pp = "cmd.exe /C \"" + cmd + "\"";
	
	LocalProcess lp(cmd);
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	
	//DUMP(lp.GetExitMessage());
	if (!tt.IsEmpty()) {
		Cout() << "FAILLED!\n";
		Cout() << tt << "\n";
		return false;
	}
	else {
		Cout() << "done in " << sw.ToString() << " seconds.\n";
		return true;
	}
}

bool Builder::BuildMSC(const String& path, const String& origPath) {
	return false;
}

bool Builder::BuildGCC(const String& path, const String& origPath) {
	String inPath = GetFileDirectory(path);
	String inTitle = GetFileTitle(path);
	String outPath = GetFileDirectory(origPath);
	String outTitle = GetFileTitle(origPath);
	String c;

	bool result = true;
	
	cppPath = bm.Compiler;
	linkPath = cppPath;

	if (optimize == " -Od")
		optimize = " -Og";
	
	String leakObj = AppendFileName(target, "leakdetect.o");

	if (!FileExists(leakObj))
		if (!CompileGCC(zPath + "codegen/leakdetect.cpp", leakObj))
			result = false;
	
	if (!CompileGCC(inPath + inTitle + ".cpp", inPath + inTitle + ".o"))
		result = false;
	
	StopWatch sw;
	String d, t, tt;
	
	Cout() << "\tlinking... ";
	
	d << QT + linkPath + QT + " " + inPath + inTitle + ".o \"" + leakObj + "\" -o \"" + outPath + outTitle + "\" -m";
	if (arch == "x64")
		d << "64";
	else
		d << "32";
	
	{
		LocalProcess lp(d);
		while (lp.Read(t)) {
			if (t.GetCount())
				tt << t;
		}
		if (tt.GetLength()) {
			Cout() << "FAILLED!\n";
			Cout() << tt << "\n";
			result = false;
		}
		else {
			Cout() << "done in " << sw.ToString() << " seconds.\n";
		}
	}

	DeleteFile(inPath + inTitle + ".o");
	DeleteFile(inPath + inTitle + ".cpp");
	
	return result;
}

/*bool Builder::BuildGCC(const String& path, const String& origPath) {
	bool result = true;
	String O = ol;
	
	String s = GetFileDirectory(path);
	String n = GetFileTitle(path);
	String s2 = GetFileDirectory(origPath);
	String n2 = GetFileTitle(origPath);
	String c;

	//c << "SET PATH=%PATH%;";
	//c << bm.Compiler << "\\bin" << ";";
	//c << " & ";

	//c << "cd \"" + s + "\" & ";
	String leak = curDir + NativePath("codegen\\posix\\gcc\\leakdetect." + arch + ".o");
	if (O == " -Od")
		O = " -Og";
	O << " ";
	c << bm.Compiler +  " -w " + s + n + ".cpp " + leak + O + " -o " + s2 + n2 + " -fpermissive -fexceptions -msse2 -m";
	if (arch == "x64")
		c << "64";
	else
		c << "32";
	c << " -x c++ -std=c++11";
	//c << " -g2 -static -x c++ -std=c++11";
	//c << " -lraylib -lopengl32 -lgdi32";
	DUMP(c);
	String t, tt;
	String pp = c;//"cmd.exe /C \"" + c + "\"";

	SetCurrentDirectory(s);

	LocalProcess lp(pp);
	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	if (!tt.IsEmpty()) {
		Cout() << "Binary generation output: " << tt << "\n";
		result = false;
	}

	//DeleteFile(s + n + ".cpp");
	
	return result;
}*/

#endif

}