#ifndef _z2c2_ErrorReporter_h_
#define _z2c2_ErrorReporter_h_

#include <Core/Core.h>

namespace Z2 {
	
using namespace Upp;

class Assembly;
class ZClass;
class Method;
class Node;
class Overload;

class ZSyntaxError: Moveable<ZSyntaxError> {
public:
	String Path;
	String Error;
	Point  ErrorPoint;
	Overload* Context;
	
	ZSyntaxError(const String& path, const Point& p, const String& error): Path(path), ErrorPoint(p), Error(error) {
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
	static void UnreachableCode(const String& path, const Point& p);
	
	static void CantAssign(const String& path, const Point& p, const String& c1, const String& c2);
	static void AssignNotLValue(const String& path, const Point& p);
	static void AssignConst(const String& path, const Point& p, const String& c);
	static void CantCreateClassVar(const String& path, const Point& p, const String& c);
	
	static void CantCall(const String& path, Point& p, Assembly& ass, ZClass* ci, Method* def, Vector<Node*>& params, int limit, bool cons = false);
	
	static void DivisionByZero(const String& path, const Point& p);
	static void IncompatOperands(const String& path, const Point& p, const String& op, const String& text, const String& text2);
	
	static void Error(const String& path, const Point& p, const String& text);
	
	static void Dup(const String& path, const Point& p, const Point& p2, const String& text, const String& text2 = "");
	static void Warning(const String& path, const Point& p, const String& text);
};

}

#endif
