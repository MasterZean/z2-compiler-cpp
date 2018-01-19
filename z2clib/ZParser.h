#ifndef _z2c_ZParser_h_
#define _z2c_ZParser_h_

#include <Core/Core.h>

using namespace Upp;

class ZSource;
class ZClass;
class Overload;
class Def;

class Context {
public:
	ZClass* C1 = nullptr;
	ZClass* C2 = nullptr;
	Point P;
	Overload* O = nullptr;
	Def* D = nullptr;
	ZSource* S = nullptr;
	
	Context* Next = nullptr;
	Context* Prev = nullptr;
};

class ZSyntaxError {
public:
	String Path;
	String Error;
	
	ZSyntaxError(const String& path, const String& error): Path(path), Error(error) {
	}
	
	void PrettyPrint(Context* con, Stream& stream);
};

class ZParser: public CParser {
public:
	enum NumberType {
		ntInvalid,
		ntInt,
		ntDWord,
		ntLong,
		ntQWord,
		ntFloat,
		ntDouble,
		ntPtrSize,
	};
	
	ZSource* Source = nullptr;
	String Mode;
	
	ZParser() {
	}
	
	ZParser(const char* ptr): CParser(ptr) {
	}
			
	Point GetPoint() {
		return Point(GetLine(), GetPos().GetColumn());
	}
	
	String Identify();
	
	String ErrorString(const Point& p, const String& text);
	void Error(const Point& p, const String& text);
	void Dup(const Point& p, const Point& p2, const String& text, const String& text2 = "");
	void Ambig(const Point& p, const Point& p2, const String& text, const String& text2 = "");
	void Warning(const Point& p, const String& text);
	
	String ExpectId();
	String ExpectZId();
	String ExpectId(const String& id);

	void Expect(char ch);
	void Expect(char ch, char ch2);
	int ExpectNum();
	
	NumberType ReadInt64(int64& oInt, double& oDub, int& base);
	
	bool IsCharConst() {
		return term[0] == '\'';
	}
	
	void SetLine(int l) {
		line = l;
	}
	
	bool IsZId();
	
	uint32 ReadChar();
	
	bool EatIf();
	bool EatElse();
	bool EatEndIf();
	bool IsElse();
	bool IsEndIf();
	void SkipBlock();
	
private:
	uint64 ReadNumber64Core(Point& p, int base);
	NumberType ReadF(Point& p, int sign, double& oDub);
	NumberType ReadI(Point& p, int sign, int64& oInt);
};

#endif