#include "ErrorReporter.h"

namespace Z2 {

#ifdef PLATFORM_POSIX

inline void SetConsoleTextAttribute(int, int) {
}

#endif

void ZSyntaxError::PrettyPrint(Stream& stream) {
#ifdef PLATFORM_WIN32

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	WORD                        m_currentConsoleAttr;
	CONSOLE_SCREEN_BUFFER_INFO   csbi;

	//retrieve and save the current attributes
	if(GetConsoleScreenBufferInfo(hConsole, &csbi))
	    m_currentConsoleAttr = csbi.wAttributes;

	int cWhite = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	int cRed = FOREGROUND_RED | FOREGROUND_INTENSITY;
	int cCyan = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	int cBlue = FOREGROUND_BLUE | FOREGROUND_GREEN;
	
#else

	int hConsole = 0;
	int m_currentConsoleAttr;
	
	int cWhite = 0;
	int cRed = 0;
	int cCyan = 0;
	int cBlue = 0;
	
#endif

	SetConsoleTextAttribute(hConsole, cWhite);

	stream << Path;
	stream << ": ";
	SetConsoleTextAttribute(hConsole, cRed);
	stream << "error:\n\t";
	
	Vector<String> v = Split(Error, '\f', false);
	int col = cWhite;
	
	for (int i = 0; i < v.GetCount(); i++) {
		SetConsoleTextAttribute(hConsole, col);
		stream << v[i];
		if (col == cWhite)
			col = cCyan;
		else
			col = cWhite;
	}
	stream << "\n";

	SetConsoleTextAttribute(hConsole, m_currentConsoleAttr);
}

void ErrorReporter::Error(const String& path, const Point& p, const String& text) {
	//ASSERT(0);
	throw ZSyntaxError(String().Cat() << path << "(" << p.x << ", " << p.y << ")", text);
}

void ErrorReporter::Dup(const String& path, const Point& p, const Point& p2, const String& text, const String& text2) {
	Error(path, p, "duplicate definition '" + text + "', previous definition was at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

void ErrorReporter::Warning(const String& path, const Point& p, const String& text) {
	Cout() << path << "(" << p.x << ", " << p.y << "): warning:\r\n\t" << text;
}

void ErrorReporter::SyntaxError(const String& path, const Point& p, const String& text) {
	Error(path, p, "syntax error: " + text + " found");
}

void ErrorReporter::InvalidNumericLiteral(const String& path, const Point& p) {
	Error(path, p, "invalid numeric literal constant");
}

void ErrorReporter::InvalidCharLiteral(const String& path, const Point& p) {
	Error(path, p, "invalid '\fChar\f' literal constant");
}

void ErrorReporter::IntegerConstantTooBig(const String& path, const Point& p) {
	Error(path, p, "integer literal constant too big");
}

void ErrorReporter::IntegerConstantTooBig(const String& path, const String& cls, const Point& p) {
	Error(path, p, "'\f" + cls + "\f' literal constant too big");
}

void ErrorReporter::FloatConstantTooBig(const String& path, const Point& p) {
	Error(path, p, "floating point constant too big");
}

void ErrorReporter::QWordWrongSufix(const String& path, const Point& p) {
	Error(path, p, "'\fQWord\f' literal constant requires 'ul' sufix");
}

void ErrorReporter::LongWrongSufix(const String& path, const Point& p) {
	Error(path, p, "'\fLong\f' literal constant requires 'l' sufix");
}

void ErrorReporter::UnsignedLeadingMinus(const String& path, const Point& p) {
	Error(path, p, "unsinged integer literal constant can't have a leading '-'");
}

void ErrorReporter::ExpectedNotFound(const String& path, const Point& p, const String& expected, const String& found) {
	Error(path, p, expected + " expected, " + found + " found");
}

void ErrorReporter::EosExpected(const String& path, const Point& p, const String& found) {
	Error(path, p, "end of statement expected, " + found + " found");
}

void ErrorReporter::IdentifierExpected(const String& path, const Point& p, const String& found) {
	Error(path, p, "identifier expected, " + found + " found");
}

void ErrorReporter::IdentifierExpected(const String& path, const Point& p, const String& id, const String& found) {
	Error(path, p, "identifier '" + id + "' expected, " + found + " found");
}

void ErrorReporter::UndeclaredIdentifier(const String& path, const Point& p, const String& id) {
	Error(path, p, "undeclared identifier '" + id + "'");
}

void ErrorReporter::DivisionByZero(const String& path, const Point& p) {
	Error(path, p, "second operand of division is 0 or equivalent");
}

void ErrorReporter::IncompatOperands(const String& path, const Point& p, const String& op, const String& text, const String& text2) {
	Error(path, p, "can't apply operator '" + op + "' on types: \n\t\t'\f" + text + "\f' and \n\t\t'\f" + text2 + "\f'");
}


}






