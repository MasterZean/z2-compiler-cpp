#include "ZParser.h"

extern char tab1[24];
extern char tab2[9];
extern char tab3[9];

#include "source.h"
#include <z2clib/Assembly.h>

String ZParser::Identify() {
	if (IsId("true"))
		return "boolean constant 'true'";
	else if (IsId("false"))
		return "boolean constant 'false'";
	else if (IsInt())
		return "integer constant '" + IntStr(ReadInt()) + "'";
	else if (IsEof())
		return "end-of-file";
	else if (IsId()) {
		if (!IsZId())
			return "keyword '" + ReadId() + "'";
		else
			return "identifier '" + ReadId() + "'";
	}
	else if (IsString())
		return "string constant";
	else {
		for (int i = 0; i < 9; i++)
			if (IsChar2(tab2[i], tab3[i])) {
				char c[3] = "  ";
				c[0] = tab2[i];
				c[1] = tab3[i];
			    return c;
			}
		for (int i = 0; i < 24; i++)
			if (IsChar(tab1[i])) {
				char c[2] = " ";
				c[0] = tab1[i];
			    return c;
			}
		return "unexpected token";
	}
}

void ZParser::Expect(char ch) {
	if (!Char(ch)) {
		Point p = GetPoint();
		Error(p, "'" + (String().Cat() << ch) + "' expected, " + Identify() + " found");
	}
}

void ZParser::Expect(char ch, char ch2) {
	if (!Char2(ch, ch2)) {
		Point p = GetPoint();
		Error(p, "'" + (String().Cat() << ch << ch2) + "' expected, " + Identify() + " found");
	}
}

String ZParser::ErrorString(const Point& p, const String& text){
	return String().Cat() << Source->Package->Path << Source->Path << "(" << p.x << ", " << p.y << ")" << Mode << ": " << text;
}

void ZParser::Error(const Point& p, const String& text) {
	//ASSERT(0);
	ASSERT(Source && Source->Package);
	throw ZSyntaxError(String().Cat() << Source->Package->Path << Source->Path << "(" << p.x << ", " << p.y << ")" << Mode, text);
}

void ZParser::Dup(const Point& p, const Point& p2, const String& text, const String& text2) {
	Error(p, "duplicate definition '" + text + "', previous definition was at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

void ZParser::Ambig(const Point& p, const Point& p2, const String& text, const String& text2) {
	Error(p, "function '" + text + "' is ambigous with one of its overloads found at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

void ZParser::Warning(const Point& p, const String& text) {
	String s;
	s << Source->Package->Path << Source->Path << "("
			<< p.x << ", " << p.y << ")" << Mode << ": " << text << "\n";
	Cout() << s;
}

String ZParser::ExpectId() {
	if (IsId())
		return ReadId();
	else {
		Point p = GetPoint();
		Error(p, "identifier expected, " + Identify() + " found");

		return "";
	}
}

String ZParser::ExpectZId() {
	if (IsZId())
		return ReadId();
	else {
		Point p = GetPoint();
		Error(p, "identifier expected, " + Identify() + " found");

		return "";
	}
}

String ZParser::ExpectId(const String& id) {
	if (IsId(id))
		return ReadId();
	else {
		Point p = GetPoint();
		Error(p, "'" + id + "' expected, " + Identify() + " found");

		return "";
	}
}

int ZParser::ExpectNum() {
	if (IsInt())
		return ReadInt();
	else {
		Point p = GetPoint();
		Error(p, "integer expected, " + Identify() + " found");

		return 0;
	}
}

bool ZParser::IsZId() {
	if (term[0] == 'i' && term[1] == 'f' && !IsAlNum(term[2]))
		return false;
	else if (term[0] == 'f') {
		if (term[1] == 'o' && term[2] == 'r') {
			if (term[3] == 'e' && term[4] == 'a' && term[5] == 'c' && term[6] == 'h' && !IsAlNum(term[7]))
				return false;
			else if (!IsAlNum(term[3]))
				return false;
		}
		else if (term[1] == 'i' && term[2] == 'n' && term[3] == 'a' && term[4] == 'l' && term[5] == 'l' && term[6] == 'y' && !IsAlNum(term[7]))
				return false;
	}
	else if (term[0] == 'e' && term[1] == 'l' && term[2] == 's' && term[3] == 'e' && !IsAlNum(term[4]))
		return false;
	else if (term[0] == 'r' && term[1] == 'e') {
		if (term[2] == 'f' && !IsAlNum(term[3]))
			return false;
		else if (term[2] == 't' && term[3] == 'u' && term[4] == 'r' && term[5] == 'n' && !IsAlNum(term[6]))
			return false;
	}
	else if (term[0] == 'v' && term[1] == 'a' && term[2] == 'l' && !IsAlNum(term[3]))
		return false;
	else if (term[0] == 'd' && term[1] == 'o' && !IsAlNum(term[2]))
		return false;
	else if (term[0] == 'w' && term[1] == 'h' && term[2] == 'i' && term[3] == 'l' && term[4] == 'e' && !IsAlNum(term[5]))
		return false;
	
	return IsId();
}

int ZParser::ReadInt64(int64& oInt, double& oDub, int& base) {
	Point p = GetPoint();
	int sign = Sgn();
	double nf = 0;
	if (*term == '0') {
		term++;
		if (*term == 'x' || *term == 'X') {
			term++;
			oInt = ReadNumber64Core(p, 16);
			base = 16;
			return ReadI(p, sign, oInt);
		}
		else if (*term == 'o' || *term == 'O') {
			term++;
			oInt = ReadNumber64Core(p, 8);
			base = 8;
			return ReadI(p, sign, oInt);
		}
		else if (*term == 'b' || *term == 'B') {
			term++;
			oInt = ReadNumber64Core(p, 2);
			base = 2;
			return ReadI(p, sign, oInt);
		}
		else if (IsAlNum(*term) && (*term != 'u' && *term != 's' && *term != 'l' && *term != 'p'))
			Error(p, "invalid numeric literal");
		else {
			if (*term == '.' && IsDigit(*(term + 1))) {
				oDub = 0;
				base = 10;
				return ReadF(p, sign, oDub);
			}
			else {
				oInt = 0;
				base = 10;
				if (IsDigit(*term))
					oInt = ReadNumber64Core(p, 10);
				return ReadI(p, sign, oInt);
			}
		}
		ASSERT(0);
		return 0;
	}
	else {
		//back = term;
		base = 10;
		oInt = ReadNumber64Core(p, 10);
		if (*term == '.' && IsDigit(*(term + 1))) {
			oDub = (double)oInt;
			return ReadF(p, sign, oDub);
		}
		else
			return ReadI(p, sign, oInt);
	}
}

uint64 ZParser::ReadNumber64Core(Point& p, int base) {
	uint64 n = 0;
	int q = ctoi(*term);
	bool expect = false;
	if(q < 0 || q >= base)
		Error(p, "invalid numeric literal");
	for(;;) {
		if (*term == '\'') {
			term++;
			continue;
		}
		int c = ctoi(*term);
		if(c < 0 || c >= base)
			break;
		uint64 n1 = n;
		n = base * n + c;
		if(n1 > n)
			Error(p, "integer constant is too big");
		if ((int64)n < 0 && (int64)n != -9223372036854775808ll) {
			expect = true;
		}
		term++;
	}

	if (expect && term[0] != 'u' && term[2] != 'l')
		Error(p, "'QWord' literal constants require a 'ul' sufix");
		
	return n;
}

union Double_t
{
    Double_t(double val) : f(val) {}
    // Portable extraction of components.
    bool Negative() const { return (i >> 63) != 0; }
    
    int64 i;
    double f;
#ifdef _DEBUG
    struct
    { // Bitfields for exploration. Do not use in production code.
        uint64 mantissa : 52;
        uint64 exponent : 11;
        uint64 sign : 1;
    } parts;
#endif
};

int ZParser::ReadF(Point& p, int sign, double& oDub) {
	term++;
	double nf = oDub;
	double q = 1;
	while(IsDigit(*term)) {
		q = q / 10;
		nf += q * (*term++ - '0');
	}

	if(Char('e') || Char('E')) {
		int exp = ReadInt();
		long double ddd = pow(10.0, exp);
		nf *= ddd;
	}
	bool f = Char('f') || Char('F');
	nf = sign * nf;
	if(!IsFin(nf))
		Error(p, "floating point constant is too big");
	oDub = nf;
	Spaces();
	
	return f ? 5 : 4;
}

int ZParser::ReadI(Point& p, int sign, int64& oInt) {
	int64 i = oInt;
	if(sign > 0 ? i > INT64_MAX : i > (uint64)INT64_MAX + 1)
		Error(p, "integer constant is too big");

	bool l = false;
	bool u = false;
	bool ps = false;
	if (*term == 'l') {
		term++;
		l = true;
		if (IsAlNum(*term))
			Error(p, "invalid numeric literal");
	}
	else if (*term == 'u') {
		term++;
		u = true;
		if (*term == 'l') {
			term++;
			l = true;
			if (IsAlNum(*term))
				Error(p, "invalid numeric literal");
		}
		else if (IsAlNum(*term))
			Error(p, "invalid numeric literal");
	}
	else if (*term == 's') {
		term++;
		if (*term == 'l') {
			term++;
			l = true;
			if (IsAlNum(*term))
				Error(p, "invalid numeric literal");
		}
		else if (IsAlNum(*term))
			Error(p, "invalid numeric literal");
	}
	else if (*term == 'p') {
		term++;
		ps = true;
	}
	if (u) {
		if (sign == -1)
			Error(p, "unsinged integer constants can't have a leading '-'");
		if (i > 4294967295 && !l)
			Error(p, "'QWord' literal constants require a 'l' sufix");
	}
	else {
		i = sign * i;
		if ((i < -2147483648LL || i > 2147483647) && !l)
			Error(p, "'Long' literal constants require a 'l' sufix");
	}
	oInt = i;
	Spaces();
	return ps ? 6 : ((u ? 1: 0) + (l ? 2 : 0));
}

uint32 ZParser::ReadChar() {
	if (term[0] != '\'')
		return -1;
	if (term[1] == '\\') {
		term += 2;
		uint32 c = -1;
		if (*term == 't') {
			c = '\t';
			term++;
		}
		else if (*term == 'n') {
			c = '\n';
			term++;
		}
		else if (*term == 'r') {
			c = '\r';
			term++;
		}
		else if (*term == 'a') {
			c = '\a';
			term++;
		}
		else if (*term == 'b') {
			c = '\b';
			term++;
		}
		else if (*term == 'f') {
			c = '\f';
			term++;
		}
		else if (*term == 'v') {
			c = '\v';
			term++;
		}
		else if (*term == '\'') {
			c = '\'';
			term++;
		}
		else if (*term == '0') {
			c = '\0';
			term++;
		}
		else if (*term == '\\') {
			c = '\\';
			term++;
		}
		else if (*term == 'u') {
			term++;
			c = 0;
			for(int i = 0; i < 6; i++) {
				uint32 cc = ctoi(*term);
				if(cc < 0 || cc >= 16)
					return -1;
				c = 16 * c + cc;
				term++;
			}
		}
		else
			return -1;
		if (*term != '\'')
			return -1;
		term += 1;
		Spaces();
		return c;
	}
	else {
		term += 1;
		uint32 c = (byte)*term;
		if (c == 0)
			return -1;
		if (c < 0x80)
			term += 1;
		else if (c < 0xC0)
			return -1;
		else if (c < 0xE0) {
			c = ((c - 0xC0) << 6) + (byte)term[1] - 0x80;
			term += 2;
		}
		else if (c < 0xF0) {
			c = ((c - 0xE0) << 12) + (((byte)term[1] - 0x80) << 6) + (byte)term[2] - 0x80;
			term += 3;
		}
		else if (c < 0xF5) {
			c = ((c - 0xE0) << 18) + (((byte)term[1] - 0x80) << 12) + (((byte)term[2] - 0x80) << 6) + (byte)term[3] - 0x80;
			term += 4;
		}
		if (*term != '\'')
			return -1;
		term += 1;
		Spaces();
		return c;
	}
}

#ifdef PLATFORM_POSIX

inline void SetConsoleTextAttribute(int, int) {
}

#endif

void ZSyntaxError::PrettyPrint(Context* con, Stream& stream) {
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

	while (con) {
		SetConsoleTextAttribute(hConsole, cWhite);

		stream << con->S->Package->Path << con->S->Path;
		stream << "(" << con->P.x << ", " << con->P.y << "): ";
		SetConsoleTextAttribute(hConsole, cCyan);
		stream << "context: ";
		SetConsoleTextAttribute(hConsole, cWhite);
		if (!con->O) {
			stream << "instantiating class '";
			SetConsoleTextAttribute(hConsole, cCyan);
			stream << con->C1->Scan.Name << "<" << con->C2->Scan.Name << ">";
			SetConsoleTextAttribute(hConsole, cWhite);
			
			stream << "'\n";
		}
		else {
			if (con->O->IsCons == 1) {
				stream << "instantiating constructor '";
				SetConsoleTextAttribute(hConsole, cCyan);
				stream << con->C1->Scan.Name << "{" << con->O->PSig << "}";
				SetConsoleTextAttribute(hConsole, cWhite);
			}
			else if (con->O->IsCons == 2) {
				stream << "instantiating constructor '";
				SetConsoleTextAttribute(hConsole, cCyan);
				stream << con->C1->Scan.Name << "." << con->O->Name << "{" << con->O->PSig << "}";
				SetConsoleTextAttribute(hConsole, cWhite);
			}
			else if (con->O->IsDest) {
				stream << "instantiating destructor '";
				SetConsoleTextAttribute(hConsole, cCyan);
				stream << con->C1->Scan.Name << ".~{}";
				SetConsoleTextAttribute(hConsole, cWhite);
			}
			else {
				stream << "instantiating method '";
				SetConsoleTextAttribute(hConsole, cCyan);
				stream << con->C1->Scan.Name << "." << con->O->Name << "(" << con->O->PSig << ")";
				SetConsoleTextAttribute(hConsole, cWhite);
			}
			
			stream << "'\n";
		}
				
		con = con->Next;
	}
	
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

bool ZParser::EatIf() {
	if (term[0] == '#' && term[1] == 'i' && term[2] == 'f') {
		term += 3;
		Spaces();
		return true;
	}
	return false;
}

bool ZParser::EatElse() {
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'l' && term[3] == 's' && term[4] == 'e') {
		term += 5;
		Spaces();
		return true;
	}
	return false;
}

bool ZParser::EatEndIf() {
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'n' && term[3] == 'd' && term[4] == 'i'  && term[5] == 'f') {
		term += 6;
		Spaces();
		return true;
	}
	return false;
}

bool ZParser::IsElse() {
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'l' && term[3] == 's' && term[4] == 'e') {
		//term += 5;
		//Spaces();
		return true;
	}
	return false;
}

bool ZParser::IsEndIf() {
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'n' && term[3] == 'd' && term[4] == 'i'  && term[5] == 'f') {
		//term += 5;
		//Spaces();
		return true;
	}
	return false;
}

void ZParser::SkipBlock() {
	while (true) {
		if (term[0] == '#' && term[1] == 'e' && term[2] == 'l' && term[3] == 's' && term[4] == 'e') {
			//term += 5;
			//Spaces();
			return;
		}
		else if (term[0] == '#' && term[1] == 'e' && term[2] == 'n' && term[3] == 'd' && term[4] == 'i'  && term[5] == 'f') {
			//term += 6;
			return;
		}
		else if (term[0] == 0)
			return;
		else if (term[0] == '\n') {
			line++;
			term++;
		}
		else
			term++;
	}
	Spaces();
}
