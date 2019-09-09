#ifndef _z2c2_ZParser_h_
#define _z2c2_ZParser_h_

#include <Core/Core.h>

using namespace Upp;

namespace Z2 {

class ZParser: public CParser {
public:
	enum NumberType {
		ntInvalid,
		ntSmall,
		ntInt,
		ntLong,
		ntByte,
		ntDWord,
		ntQWord,
		ntFloat,
		ntDouble,
		ntPtrSize,
	};
	
	String Path;

	ZParser() {
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
	
	void ExpectEndStat();
	void EatNewLines();
	
	void SkipError();
	
private:
	uint64 ReadNumber64Core(Point& p, int base);
	NumberType ReadF(Point& p, int sign, double& oDub);
	NumberType ReadI(Point& p, int sign, int64& oInt);
};

}

#endif
