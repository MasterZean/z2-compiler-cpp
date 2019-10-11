#include "ZParser.h"
#include "ErrorReporter.h"
#include "tables.h"

namespace Z2 {
	
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
		return "identifier '" + ReadZId() + "'";
	else if (IsId())
		return "keyword '" + ReadId() + "'";
	else if (IsString())
		return "string constant";
	else if (term[0] == '\n')
		return "end of statement";
	else if (term[0] <= 32)
		return "whitespace";
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

bool ZParser::IsZId() {
	if (term[0] == '@')
		return iscib(*(term + 1));
	else if (term[0] == 'a' && IsId0("alias"))
		return false;
	else if (term[0] == 'b' && IsId0("break"))
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

String ZParser::ReadZId() {
	//if(!IsId())
	//	ThrowError("missing id");
	String result;
	const char *b = term;
	const char *p = b;
	if (*p == '@')
		p++;
	while(iscid(*p))
		p++;
	term = p;
	DoSpaces();
	return String(b, (int)(uintptr_t)(p - b));
}

ZParser::NumberType ZParser::ReadInt64(int64& oInt, double& oDub, int& base) {
	Point p = GetPoint();
	int sign = Sgn();
	
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
			ErrorReporter::InvalidNumericLiteral(Path, p);
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
		ErrorReporter::InvalidNumericLiteral(Path, p);
	
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
			ErrorReporter::IntegerConstantTooBig(Path, p);
		
		if ((int64)n < 0 && n != -9223372036854775808ul) {
			expect = true;
		}
		
		term++;
	}

	if (expect && term[0] != 'u' && term[2] != 'l')
		ErrorReporter::QWordWrongSufix(Path, p);
		
	return n;
}

ZParser::NumberType ZParser::ReadI(Point& p, int sign, int64& oInt) {
	int64 i = oInt;
	if(sign > 0 ? i > INT64_MAX : i > (uint64)INT64_MAX + 1)
		ErrorReporter::IntegerConstantTooBig(Path, p);

	bool l = false;
	bool u = false;
	bool ps = false;
	
	NumberType nt = ntInt;
	
	if (*term == 'l') {
		term++;
		l = true;
		if (IsAlNum(*term))
			ErrorReporter::InvalidNumericLiteral(Path, p);
		uint64 n = i;
		if (sign == 1 && n > 9223372036854775807)
			ErrorReporter::IntegerConstantTooBig(Path, "Long", p);
		if (sign == -1 && n > 9223372036854775808ul)
			ErrorReporter::IntegerConstantTooBig(Path, "Long", p);
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
				ErrorReporter::InvalidNumericLiteral(Path, p);
			nt = ntQWord;
		}
		else if (IsDigit(*term)) {
			uint64 n = 0;
			
			while (IsDigit(*term)) {
				int c = ctoi(*term);
				if(c < 0 || c >= 10) {
					ErrorReporter::InvalidNumericLiteral(Path, p);
					break;
				}
					
				uint64 n1 = n;
				n = 10 * n + c;
				
				if(n1 > n)
					ErrorReporter::InvalidNumericLiteral(Path, p);
				
				term++;
			}
			
			if (n == 8) {
				if (i > 255)
					ErrorReporter::IntegerConstantTooBig(Path, "Byte", p);
				nt = ntByte;
			}
			else if (n == 16) {
				if (i > 65535)
					ErrorReporter::IntegerConstantTooBig(Path, "Word", p);
				nt = ntWord;
			}
			else if (n == 32) {
				if (i > 4294967295)
					ErrorReporter::IntegerConstantTooBig(Path, "DWord", p);
			}
			else
				ErrorReporter::InvalidNumericLiteral(Path, p);
		}
		else if (IsAlNum(*term))
			ErrorReporter::InvalidNumericLiteral(Path, p);
	}
	else if (*term == 's') {
		term++;
		if (*term == 'l') {
			term++;
			l = true;
			if (IsAlNum(*term))
				ErrorReporter::InvalidNumericLiteral(Path, p);
			uint64 n = i;
			if (sign == 1 && n > 9223372036854775807)
				ErrorReporter::IntegerConstantTooBig(Path, "Long", p);
			if (sign == -1 && n > 9223372036854775808ul)
				ErrorReporter::IntegerConstantTooBig(Path, "Long", p);
			nt = ntLong;
		}
		else if (IsDigit(*term)) {
			uint64 n = 0;
			
			while (IsDigit(*term)) {
				int c = ctoi(*term);
				if(c < 0 || c >= 10) {
					ErrorReporter::InvalidNumericLiteral(Path, p);
					break;
				}
					
				uint64 n1 = n;
				n = 10 * n + c;
				
				if(n1 > n)
					ErrorReporter::InvalidNumericLiteral(Path, p);
				
				term++;
			}
			
			if (n == 8) {
				int ii = sign * i;
				if (ii < -128 || ii > 127)
					ErrorReporter::IntegerConstantTooBig(Path, "Small", p);
				nt = ntSmall;
			}
			else if (n == 16) {
				int ii = sign * i;
				if (ii < -32768 || ii > 32767)
					ErrorReporter::IntegerConstantTooBig(Path, "Short", p);
				nt = ntShort;
			}
			else if (n == 32) {
				int64 ii = sign * i;
				if (ii < -2147483648 || ii > 2147483647)
					ErrorReporter::IntegerConstantTooBig(Path, "Int", p);
			}
			else
				ErrorReporter::InvalidNumericLiteral(Path, p);
		}
		else if (IsAlNum(*term))
			ErrorReporter::InvalidNumericLiteral(Path, p);
	}
	else if (*term == 'p') {
		term++;
		ps = true;
		nt = ntPtrSize;
	}
	
	if (u || ps) {
		if (sign == -1)
			ErrorReporter::UnsignedLeadingMinus(Path, p);
		if (i > 4294967295 && !l)
			ErrorReporter::QWordWrongSufix(Path, p);
	}
	else {
		i = sign * i;
		if ((i < -2147483648LL || i > 2147483647) && !l)
			ErrorReporter::LongWrongSufix(Path, p);
	}
	
	oInt = i;
	DoSpaces();
	
	return nt;
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
		ErrorReporter::FloatConstantTooBig(Path, p);
	
	oDub = nf;
	DoSpaces();
	
	return f ? ntFloat : ntDouble;
}

void ZParser::Expect(char ch) {
	if (!Char(ch)) {
		Point p = GetPoint();
		ErrorReporter::ExpectedNotFound(Path, p, "'" + (String().Cat() << ch) + "'", Identify());
	}
}

String ZParser::ExpectId() {
	if (IsId())
		return ReadId();
	else {
		Point p = GetPoint();
		ErrorReporter::IdentifierExpected(Path, p, Identify());
		
		return "";
	}
}

String ZParser::ExpectZId() {
	if (IsZId())
		return ReadZId();
	else {
		Point p = GetPoint();
		ErrorReporter::IdentifierExpected(Path, p, Identify());

		return "";
	}
}

String ZParser::ExpectId(const String& id) {
	if (IsId(id))
		return ReadId();
	else {
		Point p = GetPoint();
		ErrorReporter::IdentifierExpected(Path, p, id, Identify());

		return "";
	}
}

void ZParser::ExpectEndStat() {
	char ch = PeekChar();

	if (Char(';')) {
	}
	else if (ch != '\n' && ch != '\r' && ch != '}' && ch != ')' && ch != ']') {
		Point p = GetPoint();
		ErrorReporter::EosExpected(Path, p, Identify());
	}
}

void ZParser::SkipError() {
	while (true) {
		if (*term < 32 || *term == '}')
			break;
		term++;
	}
}

bool ZParser::WSCurrentLine() {
	if(!term)
		return false;
	
	if((byte)*term > ' ' &&
	   !(term[0] == '/' && term[1] == '/') &&
	   !(term[0] == '/' && term[1] == '*'))
		return false;
	
	for(;;) {
		if(*term == LINEINFO_ESC) {
			term++;
			fn.Clear();
			while(*term) {
				if(*term == LINEINFO_ESC) {
					++term;
					break;
				}
				if(*term == '\3') {
					line = atoi(++term);
					while(*term) {
						if(*term == LINEINFO_ESC) {
							++term;
							break;
						}
						term++;
					}
					break;
				}
				fn.Cat(*term++);
			}
			continue;
		}
		else
		if(term[0] == '/' && term[1] == '/' && skipcomments) {
			term += 2;
			while(*term && *term != '\n')
				term++;
		}
		else
		if(term[0] == '/' && term[1] == '*' && skipcomments) {
			const char* backTerm = term;
			if(nestcomments) {
				int count = 1;
				term += 2;
				while(*term) {
					if(term[0] == '*' && term[1] == '/') {
						term += 2;
						count--;
						if (count == 0)
							break;
					}
					else if(term[0] == '/' && term[1] == '*')
						count++;
					
					if(*term++ == '\n') {
						term = backTerm;
						return true;
					}
				}
			}
			else {
				term += 2;
				while(*term) {
					if(term[0] == '*' && term[1] == '/') {
						term += 2;
						break;
					}
					if(*term++ == '\n') {
						term = backTerm;
						return true;
					}
				}
			}
		}
		if(!*term)
			break;
		if((byte)*term > ' ')
			break;
		if(*term == '\n')
			return true;
		
		term++;
	}
	
	return true;
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
		
		return c;
	}
}

}