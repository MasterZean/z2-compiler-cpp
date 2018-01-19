#ifndef __BUILD_METHOD_HPP__
#define __BUILD_METHOD_HPP__

#include <Core/Core.h>

using namespace Upp;
 
struct BuildMethod: public Moveable<BuildMethod> {
	enum Type {
		btMSC,
		btGCC,
		btUnknown,
	};
	
	BuildMethod::Type Type;
	String Name;
	String Compiler;
	String Sdk;
	WithDeepCopy<Vector<String>> Lib32;
	WithDeepCopy<Vector<String>> Lib64;
	
	void Xmlize(XmlIO& xml) {
		xml("name", Name);
		if (xml.IsLoading()) {
			String s;
			xml("type", s);
			if (ToUpper(s) == "GCC")
				Type = BuildMethod::btGCC;
			else if (ToUpper(s) == "MSC")
				Type = BuildMethod::btMSC;
			else
				Type = BuildMethod::btUnknown;
		}
		else {
			if (Type == BuildMethod::btGCC) {
				String s = "GCC";
				xml("type", s);
			}
			else if (Type == BuildMethod::btMSC) {
				String s = "MSC";
				xml("type", s);
			}
			else {
				String s = "unknown";
				xml("type", s);
			}
		}
		xml("compiler", Compiler);
		xml("sdk", Sdk);
		xml("lib-x86", Lib32);
		xml("lib-x64", Lib64);
	}
	
	static void Get(Vector<BuildMethod>& methods);
	
	static String Exe(const String& exe) {
	#ifdef PLATFORM_WIN32
		return exe + ".exe";
	#endif
	#ifdef PLATFORM_POSIX
		return exe;
	#endif
	}
	
	static int ErrorCode(int code) {
	#ifdef PLATFORM_WIN32
		return code;
	#endif
	#ifdef PLATFORM_POSIX
		byte c = (byte)code;
		if (c == 0)
			c = 1;
		
		return c;
	#endif
	}
	
	static int SuccessCode(int code) {
	#ifdef PLATFORM_WIN32
		return code;
	#endif
	#ifdef PLATFORM_POSIX
		return 0;
	#endif
	}
	
	static bool IsSuccessCode(int code) {
	#ifdef PLATFORM_WIN32
		return code >= 0;
	#endif
	#ifdef PLATFORM_POSIX
		return code == 0;
	#endif
	}
	
private:
	bool TestLib(bool px86, bool px64);
	
#ifdef PLATFORM_WIN32
	bool DetectMSC7_1();
	bool DetectMSC8();
	bool DetectMSC9();
	bool DetectMSC10();
	bool DetectMSC11();
	bool DetectMSC12();
	bool DetectMSC14();
#endif
};


#endif