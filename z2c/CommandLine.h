#ifndef __COMMAND_LINE_HPP__
#define __COMMAND_LINE_HPP__

#include <Core/Core.h>

using namespace Upp;

namespace Z2 {
	
class CommandLine {
public:
	Index<String> Packages;
	String Path;
	String OutPath;
	
	String O;
	String BMName;
	String ARCH = "x86";
	
	bool CC = false;
	bool FF = false;
	bool SCU = false;
	bool LIB = false;
	bool CPP = false;
	bool BM = false;
	bool VASM = false;
	bool INT = false;
	bool LAZY = false;
	
	Point ACP = Point(-1, -1);
	
	String Class;
	
	bool Read();
};

}

#endif