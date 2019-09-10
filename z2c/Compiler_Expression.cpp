#include "Compiler.h"
#include "ErrorReporter.h"

namespace Z2 {

String strops[] = { "@add", "@sub", "@mul", "@div", "@mod", "@shl", "@shr", "@less", "@lesseq", "@more", "@moreeq", "@eq", "@neq", "@minus", "@plus", "@not", "@bitnot" };
String ops[]    = { "add",  "sub",  "mul",  "div",  "mod",  "shl",  "shr",  "less",  "lseq",    "more",  "mreq",    "eq",  "neq", "band", "bxor", "bor",  "land", "lor"  };
String opss[]   = { "+",    "-",    "*",    "/",    "%",    "<<",   ">>",   "<",     "<=",      ">",     ">=",      "==",  "!=",  "&",    "^",    "|",    "&&",   "|| "  , "=",  "+",   "-",  "!",   "~",   "++", "--", "?"  };

Node* Compiler::ParseExpression(ZClass& conCls, Overload* conOver, ZParser& parser) {
	Node* left = ParseAtom(conCls, conOver, parser);
	CParser::Pos backupPoint;
	Node* exp = ParseBin(conCls, conOver, parser, 0, left, backupPoint);
	
	return exp;
}

Node* Compiler::ParseBin(ZClass& conCls, Overload* conOver, ZParser& parser, int prec, Node* left, CParser::Pos& backupPoint, bool secondOnlyAttempt) {
	while (true) {
		int op, tempop;
		bool opc = false, tempopc;
		int p = GetPriority(parser, op, opc);
		if (p < prec)
			return left;
		
		Point opp = parser.GetPoint();
		
		Node* right = nullptr;
		
		if (secondOnlyAttempt == false) {
			parser.GetChar();
			if (opc == true)
				parser.GetChar();
			parser.Spaces();
		
			right = ParseAtom(conCls, conOver, parser);
		}
		else {
			try {
				backupPoint = parser.GetPos();
				
				parser.GetChar();
				if (opc == true)
					parser.GetChar();
				parser.Spaces();
				
				right = ParseAtom(conCls, conOver, parser);
			}
			catch(...) {
				parser.SetPos(backupPoint);
			
				return left;
			}
		}
		
		ASSERT(right->Class);
		int nextp = GetPriority(parser, tempop, tempopc);
		if (p < nextp) {
			backupPoint = parser.GetPos();
			right = ParseBin(conCls, conOver, parser, p + 1, right, backupPoint, secondOnlyAttempt);
		}
		ASSERT(right->Class);
		
		if ((op == 3 || op == 4) && right->IsZero(ass))
			ErrorReporter::DivisionByZero(conCls.Name, opp);
		
		Node* r = irg.op(left, right, OpNode::Type(op), opp);
		if (r == nullptr)
			ErrorReporter::IncompatOperands(conCls.Name, opp, opss[op], left->Class->Name, right->Class->Name);
		
		ASSERT(r->Class);
		left = r;
	}
}

Node* Compiler::ParseAtom(ZClass& conCls, Overload* conOver, ZParser& parser) {
	Node* exp = nullptr;
	
	if (parser.IsInt())
		exp = ParseNumeric(conCls, parser);
	else if (parser.IsZId() || parser.IsChar('@'))
		exp = ParseId(conCls, conOver, parser);
	else {
		Point p = parser.GetPoint();
		ErrorReporter::SyntaxError(conCls.Name, p, parser.Identify());
	}
	
	return exp;
}

Node* Compiler::ParseId(ZClass& conCls, Overload* conOver, ZParser& parser) {
	String s;
	
	if (parser.Char('@'))
		s = "@" + parser.ExpectZId();
	else
		s = parser.ExpectZId();

	if (conOver != nullptr) {
		for (int j = 0; j < conOver->Params.GetCount(); j++) {
			if (conOver->Params[j].Name == s)
				return irg.mem(conOver->Params[j]);
		}

		for (int j = 0; j < conOver->Blocks.GetCount(); j++) {
			Block& b = conOver->Blocks[j];
			for (int k = 0; k < b.Variables.GetCount(); k++)
				if (b.Variables[k]->Name == s)
					return irg.mem(*b.Variables[k]);
		}
	}
	
	ErrorReporter::UndeclaredIdentifier(conCls.Name, parser.GetPoint(), parser.ReadId());
	
	return nullptr;
}

Node* Compiler::ParseNumeric(ZClass& conCls, ZParser& parser) {
	Node* exp = nullptr;
	
	int64 oInt;
	double oDub;
	
	int base = 10;
	int type = parser.ReadInt64(oInt, oDub, base);

	if (type == ZParser::ntInt)
		exp = irg.constIntSigned(oInt, base);
	else if (type == ZParser::ntDWord)
		exp = irg.constIntUnsigned(oInt, base);
	else if (type == ZParser::ntLong)
		exp = irg.constIntSigned(oInt, base, ass.CLong);
	else if (type == ZParser::ntQWord)
		exp = irg.constIntUnsigned(oInt, base, ass.CQWord);
	else if (type == ZParser::ntSmall)
		exp = irg.constIntUnsigned(oInt, base, ass.CSmall);
	else if (type == ZParser::ntShort)
		exp = irg.constIntUnsigned(oInt, base, ass.CShort);
	else if (type == ZParser::ntByte)
		exp = irg.constIntUnsigned(oInt, base, ass.CByte);
	else if (type == ZParser::ntWord)
		exp = irg.constIntUnsigned(oInt, base, ass.CWord);
	else if (type == ZParser::ntDouble)
		exp = irg.constFloatDouble(oDub);
	else if (type == ZParser::ntFloat)
		exp = irg.constFloatSingle(oDub);
	else if (type == ZParser::ntPtrSize)
		exp = irg.constIntSigned(oInt, base, ass.CPtrSize);
	else
		ASSERT_(0, "Error in parse int");
	
	return exp;
}

int Compiler::GetPriority(CParser& parser, int& op, bool& opc) {
	Point p = Point(-1, -1);

	if (parser.IsChar3('<', '<', '='))
		return -1;
	if (parser.IsChar3('>', '>', '='))
		return -1;
	if (parser.IsChar2('+', '='))
		return -1;
	if (parser.IsChar2('-', '='))
		return -1;
	if (parser.IsChar2('<', '-'))
		return -1;
	if (parser.IsChar2('*', '='))
		return -1;
	if (parser.IsChar2('/', '='))
		return -1;
	if (parser.IsChar2('%', '='))
		return -1;
	if (parser.IsChar2('&', '='))
		return -1;
	if (parser.IsChar2('^', '='))
		return -1;
	if (parser.IsChar2('|', '='))
		return -1;
	else if (parser.IsChar2('<', '<'))
		p = Point( 5, 210 - 20);
	else if (parser.IsChar2('>', '>'))
		p = Point( 6, 210 - 20);
	else if (parser.IsChar2('<', '='))
		p = Point( 8, 210 - 30);
	else if (parser.IsChar2('>', '='))
		p = Point(10, 210 - 30);
	else if (parser.IsChar2('=', '='))
		p = Point(11, 210 - 40);
	else if (parser.IsChar2('!', '='))
		p = Point(12, 210 - 40);
	else if (parser.IsChar2('&', '&'))
		p = Point(16, 210 - 80);
	else if (parser.IsChar2('|', '|'))
		p = Point(17, 210 - 90);

	if (p.y != -1)
		opc = true;
	else
		p = OPS[parser.PeekChar()];

	op = p.x;
	return p.y;
}

void Compiler::SetupTables() {
	for (int i = 0; i < 255; i++)
		OPS[i] = Point(-1, -1);
	OPS['*'] = Point( 2, 210 -  0);
	OPS['/'] = Point( 3, 210 -  0);
	OPS['%'] = Point( 4, 210 -  0);
	OPS['+'] = Point( 0, 210 - 10);
	OPS['-'] = Point( 1, 210 - 10);
	OPS['<'] = Point( 7, 210 - 30);
	OPS['>'] = Point( 9, 210 - 30);
	OPS['&'] = Point(13, 210 - 50);
	OPS['^'] = Point(14, 210 - 60);
	OPS['|'] = Point(15, 210 - 70);
	
	Zero(OPCONT);
	OPCONT['('] = true;
	OPCONT['{'] = true;
	OPCONT['.'] = true;
	OPCONT['*'] = true;
	OPCONT['+'] = true;
	OPCONT['-'] = true;
	OPCONT['<'] = true;
	OPCONT['['] = true;
}

Point Compiler::OPS[256];
bool Compiler::OPCONT[256];

}

