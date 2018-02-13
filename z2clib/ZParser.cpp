#include "ZParser.h"

extern char tab1[24];
extern char tab2[9];
extern char tab3[9];

#include "Source.h"
#include "Assembly.h"

String ZParser::Identify() {
	if (IsId("true"))
		return "boolean constant 'true'";
	else if (IsId("false"))
		return "boolean constant 'false'";
	else if (IsInt())
		return "integer constant '" + IntStr(ReadInt()) + "'";
	else if (IsEof())
		return "end-of-file";
	else if (IsZId())
		return "keyword '" + ReadId() + "'";
	else if (IsId())
		return "identifier '" + ReadId() + "'";
	else if (IsString())
		return "string constant";
	else if (term[0] == '\n')
		return "end of statement";
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
	if (term[0] == 'b' && IsId0("break"))
		return false;
	else if (term[0] == 'c' && IsId0("case"))
		return false;
	else if (term[0] == 'c' && IsId0("catch"))
		return false;
	else if (term[0] == 'c' && IsId0("const"))
		return false;
	else if (term[0] == 'c' && IsId0("continue"))
		return false;
	else if (term[0] == 'c' && IsId0("class"))
		return false;
	else if (term[0] == 'd' && IsId0("default"))
		return false;
	else if (term[0] == 'd' && IsId0("def"))
		return false;
	else if (term[0] == 'd' && IsId0("do"))
		return false;
	else if (term[0] == 'e' && IsId0("enum"))
		return false;
	else if (term[0] == 'e' && IsId0("else"))
		return false;
	else if (term[0] == 'f' && IsId0("for"))
		return false;
	else if (term[0] == 'f' && IsId0("finally"))
		return false;
	else if (term[0] == 'f' && IsId0("foreach"))
		return false;
	else if (term[0] == 'f' && IsId0("func"))
		return false;
	else if (term[0] == 'g' && IsId0("goto"))
		return false;
	else if (term[0] == 'g' && IsId0("get"))
		return false;
	else if (term[0] == 'i' && IsId0("if"))
		return false;
	else if (term[0] == 'i' && IsId0("in"))
		return false;
	else if (term[0] == 'm' && IsId0("move"))
		return false;
	else if (term[0] == 'n' && IsId0("new"))
		return false;
	else if (term[0] == 'n' && IsId0("namespace"))
		return false;
	else if (term[0] == 'o' && IsId0("override"))
		return false;
	else if (term[0] == 'p' && IsId0("private"))
		return false;
	else if (term[0] == 'p' && IsId0("protected"))
		return false;
	else if (term[0] == 'p' && IsId0("public"))
		return false;
	else if (term[0] == 'p' && IsId0("property"))
		return false;
	else if (term[0] == 'r' && IsId0("ref"))
		return false;
	else if (term[0] == 'r' && IsId0("return"))
		return false;
	else if (term[0] == 's' && IsId0("static"))
		return false;
	else if (term[0] == 's' && IsId0("set"))
		return false;
	else if (term[0] == 's' && IsId0("switch"))
		return false;
	else if (term[0] == 't' && IsId0("this"))
		return false;
	else if (term[0] == 't' && IsId0("try"))
		return false;
	else if (term[0] == 't' && IsId0("throw"))
		return false;
	else if (term[0] == 'u' && IsId0("using"))
		return false;
	else if (term[0] == 'v' && IsId0("virtual"))
		return false;
	else if (term[0] == 'v' && IsId0("val"))
		return false;
	else if (term[0] == 'w' && IsId0("while"))
		return false;
	else if (term[0] == 'w' && IsId0("with"))
		return false;
	
	return IsId();
}

ZParser::NumberType ZParser::ReadInt64(int64& oInt, double& oDub, int& base) {
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
		
		return ntInvalid;
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

ZParser::NumberType ZParser::ReadF(Point& p, int sign, double& oDub) {
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
	
	return f ? ntFloat : ntDouble;
}

ZParser::NumberType ZParser::ReadI(Point& p, int sign, int64& oInt) {
	int64 i = oInt;
	if(sign > 0 ? i > INT64_MAX : i > (uint64)INT64_MAX + 1)
		Error(p, "integer constant is too big");

	bool l = false;
	bool u = false;
	bool ps = false;
	
	NumberType nt = ntInt;
	
	if (*term == 'l') {
		term++;
		l = true;
		if (IsAlNum(*term))
			Error(p, "invalid numeric literal");
		nt = ntLong;
	}
	else if (*term == 'u') {
		term++;
		u = true;
		nt = ntDWord;
		if (*term == 'l') {
			term++;
			l = true;
			if (IsAlNum(*term))
				Error(p, "invalid numeric literal");
			nt = ntQWord;
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
			nt = ntLong;
		}
		else if (IsAlNum(*term))
			Error(p, "invalid numeric literal");
	}
	else if (*term == 'p') {
		term++;
		ps = true;
		nt = ntPtrSize;
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
	
	return nt;
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

	Context* cn = con;
	
	while (cn) {
		if (cn->P.x < 0) {
			cn->P.x = -cn->P.x;
			cn->P.y = -cn->P.y;
			
			con = cn;
			
			break;
		}
		
		cn = cn->Next;
	}
	
	if (cn == nullptr) {
		while (con) {
			if (con->Next == nullptr && con->O && con->O->Class().FromTemplate)
				break;
			con = con->Next;
		}
	}
	
	while (con != nullptr) {
		Point pt = con->P;
		if (con->P.x < 0 && con->Prev)
			pt = con->Prev->P;
		
		SetConsoleTextAttribute(hConsole, cWhite);

		stream << con->S->Package->Path << con->S->Path;
		stream << "(" << pt.x << ", " << pt.y << "): ";
		SetConsoleTextAttribute(hConsole, cCyan);
		stream << "context: ";
		SetConsoleTextAttribute(hConsole, cWhite);
		if (!con->O && !con->D) {
			stream << "instantiating class '";
			SetConsoleTextAttribute(hConsole, cCyan);
			stream << con->C1->Scan.Name << "<" << con->C2->Scan.Name << ">";
			SetConsoleTextAttribute(hConsole, cWhite);
			
			stream << "'\n";
		}
		else {
			if (con->D) {
				stream << "calling method '";
				SetConsoleTextAttribute(hConsole, cCyan);
				stream << con->C1->Scan.Name << "." << con->D->Name/* << "(" << con->O->PSig << ")"*/;
				SetConsoleTextAttribute(hConsole, cWhite);
			}
			else if (con->O->IsCons == 1) {
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
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'l' && term[3] == 's' && term[4] == 'e')
		return true;
	
	return false;
}

bool ZParser::IsEndIf() {
	if (term[0] == '#' && term[1] == 'e' && term[2] == 'n' && term[3] == 'd' && term[4] == 'i'  && term[5] == 'f')
		return true;

	return false;
}

void ZParser::SkipBlock() {
	while (true) {
		if (term[0] == '#' && term[1] == 'e' && term[2] == 'l' && term[3] == 's' && term[4] == 'e')
			return;
		else if (term[0] == '#' && term[1] == 'e' && term[2] == 'n' && term[3] == 'd' && term[4] == 'i'  && term[5] == 'f')
			return;
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

void ZParser::ExpectEndStat() {
	if (skipnewlines)
		Expect(';');
	else {
		if (Char(';')) {
			skipnewlines = true;
			Spaces();
			skipnewlines = false;
		}
		else if (PeekChar() == '\n') {
			skipnewlines = true;
			Spaces();
			skipnewlines = false;
		}
		else {
			Point p = GetPoint();
			Error(p, "';' expected, " + Identify() + " found");
		}
	}
}

void ZParser::EatNewlines() {
	bool b = skipnewlines;
	skipnewlines = true;
	Spaces();
	skipnewlines = b;
}