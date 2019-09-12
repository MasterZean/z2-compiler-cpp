#ifndef _z2c2_ErrorReporter_h_
#define _z2c2_ErrorReporter_h_

#include <Core/Core.h>

using namespace Upp;

namespace Z2 {

class ZSyntaxError: Moveable<ZSyntaxError> {
public:
	String Path;
	String Error;
	
	ZSyntaxError(const String& path, const String& error): Path(path), Error(error) {
	}
	
	void PrettyPrint(Stream& stream);
};

class ErrorReporter {
public:
	static void SyntaxError(const String& path, const Point& p, const String& text);
	
	static void InvalidNumericLiteral(const String& path, const Point& p);
	static void InvalidCharLiteral(const String& path, const Point& p);
	static void IntegerConstantTooBig(const String& path, const Point& p);
	static void IntegerConstantTooBig(const String& path, const String& cls, const Point& p);
	static void FloatConstantTooBig(const String& path, const Point& p);
	static void QWordWrongSufix(const String& path, const Point& p);
	static void LongWrongSufix(const String& path, const Point& p);
	static void UnsignedLeadingMinus(const String& path, const Point& p);
	
	static void ExpectedNotFound(const String& path, const Point& p, const String& expected, const String& found);
	static void EosExpected(const String& path, const Point& p, const String& found);
	
	static void IdentifierExpected(const String& path, const Point& p, const String& found);
	static void IdentifierExpected(const String& path, const Point& p, const String& id, const String& found);
	static void UndeclaredIdentifier(const String& path, const Point& p, const String& id);
	static void UndeclaredClass(const String& path, const Point& p, const String& id);
	
	static void CantAssign(const String& path, const Point& p, const String& c1, const String& c2);
	static void CantCreateClassVar(const String& path, const Point& p, const String& c);
	
	static void DivisionByZero(const String& path, const Point& p);
	static void IncompatOperands(const String& path, const Point& p, const String& op, const String& text, const String& text2);
	
	static void Error(const String& path, const Point& p, const String& text);
	
	static void Dup(const String& path, const Point& p, const Point& p2, const String& text, const String& text2 = "");
	static void Warning(const String& path, const Point& p, const String& text);
};

}

#endif
