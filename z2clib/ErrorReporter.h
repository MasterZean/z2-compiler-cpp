#ifndef _z2clibex_ErrorReporter_h_
#define _z2clibex_ErrorReporter_h_

#include <z2clib/source.h>

class Overload;
class Constant;
class Def;
class Node;
class ObjectType;
class Assembly;

class ErrorReporter {
public:
	static void CantAccess(const ZSource& src, const Point& p, Overload& over, const String& cls);
	static void CantAccess(const ZSource& src, const Point& p, Constant& over);
	
	static void Error(const ZSource& src, const Point& p, const String& text);
	static void Error(const ZClass& cls, const Point& p, const String& text);
	
	static void Dup(const ZSource& src, const Point& p, const Point& p2, const String& text, const String& text2 = "");
	
	static void CallError(const ZClass& cls, Point& p, Assembly& ass, ObjectType* ci, Def* def, Vector<Node*>& params, bool cons = false);

	static void ErrItemCountMissing(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "\f'" + text + "'\f" + " can't be instantiated without an item count.");
	}
	
	static void ErrItemCountNegative(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "\f'" + text + "'\f" + " can only be instantiated with a greater than 0 item count.");
	}
	
	static void ErrItemCountNotInteger(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "\f'" + text + "'\f" + " can only be instantiated with a integer item count.");
	}
	
	static void ErrNotCompileTime(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "Compile time expression expected.");
	}
	
	static void ErrCArrayLiteralQualified(const ZClass& cls, const Point& p, const String& text, const String& text2) {
		Error(cls, p, "Fully qualified class '\f" + text + "\f' used as a vector literal. Remove the numeric qualifier: '\f" + text2 + "\f'.");
	}
	
	static void ErrCArrayNoQual(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "Explicitly specified type of \f'" + text + "'\f" + " must include an item count.");
	}
	
	static void ErrCArrayMoreElements(const ZClass& cls, const Point& p, const String& text, int c1, int c2) {
		Error(cls, p, "Literal '\f" + text + "\f' declared with an element count of '" + IntStr(c1) + "' initialized with '" + IntStr(c2) + "' (more) elements.");
	}
	
	static void ErrCArrayLessElements(const ZClass& cls, const Point& p, const String& text, int c1, int c2) {
		Error(cls, p, "Literal '\f" + text + "\f' declared with an element count of '" + IntStr(c1) + "' initialized with '" + IntStr(c2) + "' (less) elements.");
	}
	
	static void ErrEllipsisNocount(const ZClass& cls, const Point& p, const String& text) {
		Error(cls, p, "Vector literal '\f" + text + "\f' has ellipsis but no explicit item count.");
	}
	
	static void ErrIncompatOp(const ZClass& cls, const Point& p, const String& op, const String& text, const String& text2) {
		Error(cls, p, "Can't apply operator '" + op + "' on types: \n\t\t'\f" + text + "\f' and \n\t\t'\f" + text2 + "\f'");
	}
};

#endif
