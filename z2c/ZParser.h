#ifndef _z2c2_ZParser_h_
#define _z2c2_ZParser_h_

#include <Core/Core.h>

namespace Z2 {

using namespace Upp;

class ZParser: public CParser {
public:
	enum NumberType {
		ntInvalid,
		ntSmall,
		ntShort,
		ntInt,
		ntLong,
		ntByte,
		ntWord,
		ntDWord,
		ntQWord,
		ntFloat,
		ntDouble,
		ntPtrSize,
	};
	
	String Path;

	ZParser() {
		skipspaces = false;
	}
	
	ZParser(const char* ptr): CParser(ptr, false) {
	}
			
	Point GetPoint() {
		return Point(GetLine(), GetPos().GetColumn());
	}
	
	String Identify();
	
	bool IsZId();
	
	NumberType ReadInt64(int64& oInt, double& oDub, int& base);
	
	void Expect(char ch);
	String ExpectId();
	String ExpectZId();
	String ExpectId(const String& id);
	
	void ExpectEndStat();
	void SkipError();
	
	bool WS() {
		return Spaces();
	}
	
	bool WSCurrentLine();
	
	bool IsCharConst() {
		return term[0] == '\'';
	}
	
	uint32 ReadChar();
	
private:
	uint64 ReadNumber64Core(Point& p, int base);
	NumberType ReadF(Point& p, int sign, double& oDub);
	NumberType ReadI(Point& p, int sign, int64& oInt);
};

}

#endif
