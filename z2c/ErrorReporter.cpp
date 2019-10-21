#include "ErrorReporter.h"
#include "Assembly.h"
#include "Node.h"

namespace Z2 {
	
String NL = "\r\n";

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

	if (Context) {
		SetConsoleTextAttribute(hConsole, cWhite);
		stream << Path;
		stream << "(" << Context->NamePoint.x << ", " << Context->NamePoint.y << ")";
		stream << ": ";
				
		stream << "context: ";
		
		SetConsoleTextAttribute(hConsole, m_currentConsoleAttr);
		stream << "compiling method: ";
		
		SetConsoleTextAttribute(hConsole, cWhite);
		if (Context->IsCons)
			stream << "func ";
		else
			stream << "def ";
		
		SetConsoleTextAttribute(hConsole, cCyan);
		stream << Context->Name();
		SetConsoleTextAttribute(hConsole, cWhite);
		stream <<"(" << Context->Signature << ")\n";
	}
	
	SetConsoleTextAttribute(hConsole, cWhite);

	stream << Path;
	stream << "(" << ErrorPoint.x << ", " << ErrorPoint.y << ")";
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
	throw ZSyntaxError(path, p, text);
}

void ErrorReporter::Dup(const String& path, const Point& p, const Point& p2, const String& text, const String& text2) {
	Error(path, p, "duplicate definition '" + text + "', previous definition was at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

ZSyntaxError ErrorReporter::DupObject(const String& path, const Point& p, const Point& p2, const String& text, const String& text2) {
	return ZSyntaxError(path, p, "duplicate definition '" + text + "', previous definition was at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

void ErrorReporter::Warning(const String& path, const Point& p, const String& text) {
	Cout() << path << "(" << p.x << ", " << p.y << "): warning:" << NL << "\t" << text << NL;
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

void ErrorReporter::ExpectCT(const String& path, const Point& p) {
	Error(path, p, "expression can't be evaluated at compile time");
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

void ErrorReporter::UndeclaredIdentifier(const String& path, const Point& p, const String& c1, const String& c2) {
	Error(path, p, "class '\f" + c1 + "\f' does not have a member called '" + c2 + "'");
}

void ErrorReporter::UndeclaredClass(const String& path, const Point& p, const String& id) {
	Error(path, p, "undeclared class '\f" + id + "\f'");
}

void ErrorReporter::UnreachableCode(const String& path, const Point& p) {
	Error(path, p, "unreachable code after function return statement");
}

void ErrorReporter::ClassMustBeInstanciated(const String& path, const Point& p, const String& c) {
	Error(path, p, "class '\f" + c + "\f' must be instantiated with a class in this context");
}

void ErrorReporter::CantAssign(const String& path, const Point& p, const String& c1, const String& c2) {
	Error(path, p, "can't assign '\f" + c2 + "\f' instance to '\f" + c1 + "\f' instance");
}

void ErrorReporter::AssignNotLValue(const String& path, const Point& p) {
	Error(path, p, "left hand side of the assignment is not a L-value");
}

void ErrorReporter::AssignConst(const String& path, const Point& p, const String& c) {
	Error(path, p, "can't assign to const '\f" + c + "\f' instance");
}

void ErrorReporter::CantCreateClassVar(const String& path, const Point& p, const String& c) {
	Error(path, p, "can't create variable of type '\f" + c + "\f'");
}

void ErrorReporter::CondNotBool(const String& path, const Point& p, const String& c) {
	Error(path, p, "condition must be '\fBool\f', class '\f" + c + "\f' found");
}

void ErrorReporter::DivisionByZero(const String& path, const Point& p) {
	Error(path, p, "second operand of division is 0 or equivalent");
}

void ErrorReporter::IncompatOperands(const String& path, const Point& p, const String& op, const String& text, const String& text2) {
	Error(path, p, "can't apply operator '" + op + "' on types: " << NL << "\t\t'\f" + text + "\f' and " << NL << "\t\t'\f" + text2 + "\f'");
}

void ErrorReporter::IncompatUnary(const String& path, const Point& p, const String& text, const String& text2) {
	Error(path, p, "can't apply unary " + text + " on type '\f" + text2 + "\f'");
}

void ErrorReporter::SomeOverloadsBad(const String& path, const Point& p, const String& f) {
	Error(path, p, "can't call method '" + f + "' because errors were encountered in at least one of its overloads");
}

void ErrorReporter::CantCall(const String& path, Point& p, Assembly& ass, ZClass* ci, Method* def, Vector<Node*>& params, int limit, bool cons) {
	String s;
	
	if (cons) {
		s << "Class '\f" << ci->Name << "\f' does not have a constructor" << NL << "\t\t";
	}
	else {
		String z = ci->Name;
		s << "Class '\f" << z << "\f' does not have an overload" << NL << "\t\t";
		if (limit == 2)
			s << "func ";
		else if (limit == 1)
			s << "def  ";
		
		if (def)
			s << def->Name;
		else
			s << z;
	}
	
	if (cons)
		s << "{";
	else
		s << "(";
	
	for (int k = 0; k < params.GetCount(); k++) {
		s << params[k]->Class->Name;
		if (k < params.GetCount() - 1)
			s << ", ";
	}
	
	if (cons)
		s << "}" << NL;
	else
		s << ")" << NL;
	
	//if (def && def->IsTemplate)
	//	s << "Class \f" << ass.ClassToString(&i) <<"\f has an incompatible template '" << def->Name << "'";
	//else if (def) {
		s << "\texisting overloads" << NL;
		for (int i = 0; i < def->Overloads.GetCount(); i++) {
			Overload& ol = def->Overloads[i];
			
			//if (ol.IsDeleted)
			//	continue;
			
			if (ol.IsCons == 1)
				s << "\t\t" << "{" << ol.Signature << "}" << NL;
			else if (ol.IsCons == 2)
				s << "\t\t" << ol.Name() << "{" << ol.Signature << "}" << NL;
			else {
				s << "\t\t";
				if (ol.IsConst)
					s << "func ";
				else
					s << "def  ";
				s << ol.Name() << "(" << ol.Signature << ")" << NL;
			}
		}
	//}
	
	Error(path, p, s);
}

void ErrorReporter::AmbigError(const String& path, Point& p, Assembly& ass, ZClass* ci, Method* def, Vector<Node*>& params, int score) {
	String s;
	
	s << "class '\f" << ci->Name << "\f': when trying to match overload" << NL << "\t\t";
	if (def)
		s << def->Name;
	else
		s << ci->Name;
	s << "(";
	
	for (int k = 0; k < params.GetCount(); k++) {
		s << params[k]->Class->Name;
		if (k < params.GetCount() - 1)
			s << ", ";
	}
	s << ")" << NL;
	/*if (def && def->IsTemplate)
		s << "class \f" << ci->Name <<"\f has an incompatible template '" << def->Name << "'";
	else */if (def) {
		s << "\tgot ambiguity between" << NL;
		String z;
		for (int i = 0; i < def->Overloads.GetCount(); i++) {
			Overload& ol = def->Overloads[i];
			bool found = false;
			if (ol.Score == score)
				found = true;
			
			String dd;
			dd << "\t\t" << ol.Name() << "(";
			dd << ol.Signature;
			dd << ")" << NL;
			
			if (found)
				z << dd;
			//DUMP(dd);
		}
		
		if (z.GetCount() == 0) {
			for (int i = 0; i < def->Overloads.GetCount(); i++)
				for (int j = 0; j < i; j++) {
					Overload& ol = def->Overloads[i];
					Overload& ol2 = def->Overloads[j];
					
					if (ol.Signature == ol2.Signature) {
						z << "\t\t" << ol.Name() << "(";
						z << ol.Signature;
						z << ")" << NL;
						
						z << "\t\t" << ol2.Name() << "(";
						z << ol2.Signature;
						z << ")" << NL;
					}
				}
		}
		
		s << z;
	}

	Error(path, p, s);
}

}
