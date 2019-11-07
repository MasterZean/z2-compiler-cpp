#include "Compiler.h"
#include "ErrorReporter.h"
#include "OverloadResolver.h"
#include "tables.h"

namespace Z2 {

String strops[] = { "@add", "@sub", "@mul", "@div", "@mod", "@shl", "@shr", "@less", "@lesseq", "@more", "@moreeq", "@eq", "@neq", "@minus", "@plus", "@not", "@bitnot" };
String ops[]    = { "add",  "sub",  "mul",  "div",  "mod",  "shl",  "shr",  "less",  "lseq",    "more",  "mreq",    "eq",  "neq", "band", "bxor", "bor",  "land", "lor"  };

String CLS_STR = "class";

Node* Compiler::ParseExpression(ZClass& conCls, Overload* conOver, ZParser& parser) {
	Node* left = ParseAtom(conCls, conOver, parser);
	parser.WSCurrentLine();
	
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
			parser.WS();
		
			right = ParseAtom(conCls, conOver, parser);
			parser.WSCurrentLine();
		}
		else {
			try {
				backupPoint = parser.GetPos();
				
				parser.GetChar();
				if (opc == true)
					parser.GetChar();
				parser.WS();
				
				right = ParseAtom(conCls, conOver, parser);
				parser.WSCurrentLine();
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
			ErrorReporter::IncompatOperands(conCls.Name, opp, TabOpString[op], left->Class->Name, right->Class->Name);
		
		ASSERT(r->Class);
		left = r;
	}
}

Node* Compiler::ParseAtom(ZClass& conCls, Overload* conOver, ZParser& parser) {
	Node* exp = nullptr;
	Point p = parser.GetPoint();
	
	if (parser.IsInt())
		exp = ParseNumeric(conCls, parser);
	else if (parser.Id("true"))
		exp = irg.constBool(true);
	else if (parser.Id("false"))
		exp = irg.constBool(false);
	else if (parser.Char('-')) {
		Node* node = ParseAtom(conCls, conOver, parser);
		exp = irg.opMinus(node);
		if (exp == nullptr)
			ErrorReporter::IncompatUnary(conCls.Name, p, "'-' ('@minus')",  node->Class->Name);
	}
	else if (parser.Char2('+', '+')) {
		Point p = parser.GetPoint();
		
		exp = ParseAtom(conCls, conOver, parser);
		if (!exp->IsAddressable || exp->IsReadOnly)
			ErrorReporter::NotLValue(conCls.Name, p);
		
		exp = irg.inc(exp, true);
		
		ASSERT(exp->Class);
		
		return exp;
	}
	else if (parser.Char2('-', '-')) {
		Point p = parser.GetPoint();
		
		exp = ParseAtom(conCls, conOver, parser);
		if (!exp->IsAddressable || exp->IsReadOnly)
			ErrorReporter::NotLValue(conCls.Name, p);
		
		exp = irg.dec(exp, true);
		
		ASSERT(exp->Class);
		
		return exp;
	}
	else if (parser.Char('+')) {
		Node* node = ParseAtom(conCls, conOver, parser);
		exp = irg.opPlus(node);
		if (exp == nullptr)
			ErrorReporter::IncompatUnary(conCls.Name, p, "'+' ('@plus')",  node->Class->Name);
	}
	else if (parser.Char('!')) {
		Node* node = ParseAtom(conCls, conOver, parser);
		exp = irg.opNot(node);
		if (exp == nullptr)
			ErrorReporter::IncompatUnary(conCls.Name, p, "'!' ('@not')",  node->Class->Name);
	}
	else if (parser.Char('~')) {
		Node* node = ParseAtom(conCls, conOver, parser);
		exp = irg.opBitNot(node);
		if (exp == nullptr)
			ErrorReporter::IncompatUnary(conCls.Name, p, "'~' ('@bitnot')",  node->Class->Name);
	}
	else if (parser.IsZId())
		exp = ParseId(conCls, conOver, conOver, parser);
	else if (parser.IsCharConst()) {
		Point p = parser.GetPoint();
		
		uint32 ch = parser.ReadChar();
		if (ch == -1)
			ErrorReporter::InvalidCharLiteral(conCls.Name, p);
		
		exp = irg.constChar(ch);
	}
	else if (parser.Char2(':', ':')) {
		exp = ParseId(conCls, conOver, nullptr, parser);
	}
	else if (parser.Char('(')) {
		Node* node = ParseExpression(conCls, conOver, parser);
		parser.Expect(')');
		exp = irg.list(node);
	}
	else {
		Point p = parser.GetPoint();
		ErrorReporter::SyntaxError(conCls.Name, p, parser.Identify());
	}
	
	while (OPCONT[parser.PeekChar()]) {
		Point p = parser.GetPoint();

		if (parser.Char('{')) {
			parser.OpenCB++;
			parser.WS();
			if (exp->IsLiteral && exp->Class == ass.CCls)
				exp = ParseTemporary(conCls, conOver, parser, p, ass.Classes[(int)exp->IntVal]);
		}
		else if (parser.Char('.')) {
			ASSERT(exp->Class);

			if (exp->Class == ass.CCls && exp->IsLiteral && ass.Classes[(int)exp->IntVal].IsTemplate)
				ErrorReporter::ClassMustBeInstanciated(conCls.Name, p, ass.Classes[(int)exp->IntVal].Name);

			exp = ParseDot(conCls, conOver, parser, exp);
			
			ASSERT(exp->Class);
		}
		else if (parser.Char2('+', '+')) {
			if (!exp->IsAddressable || exp->IsReadOnly)
				ErrorReporter::NotLValue(conCls.Name, p);
			
			Node* op = irg.inc(exp);
			exp = op;
			
			ASSERT(exp->Class);
		}
		else if (parser.Char2('-', '-')) {
			if (!exp->IsAddressable || exp->IsReadOnly)
				ErrorReporter::NotLValue(conCls.Name, p);
			
			Node* op = irg.dec(exp);
			exp = op;
			
			ASSERT(exp->Class);
		}
		else
			break;
	}
	
	return exp;
}

Node* Compiler::ParseId(ZClass& conCls, Overload* conOver, Overload* searchOver, ZParser& parser) {
	Point p = parser.GetPoint();
	String s = parser.ReadZId();
	
	// local variables and parameters
	if (searchOver != nullptr) {
		for (int j = 0; j < searchOver->Params.GetCount(); j++) {
			if (searchOver->Params[j].Name == s)
				return irg.mem(searchOver->Params[j]);
		}

		for (int j = 0; j < searchOver->Blocks.GetCount(); j++) {
			Block& b = searchOver->Blocks[j];
			for (int k = 0; k < b.Variables.GetCount(); k++)
				if (b.Variables[k]->Name == s)
					return irg.mem(*b.Variables[k]);
		}
	}
	
	int i = conCls.Methods.Find(s);
	if (i != -1) {
		parser.Expect('(');
		Vector<Node*> params;
		GetParams(params, conCls, conOver, parser);
		
		int goodOvers = 0;
		Method& m = conCls.Methods[i];
		for (int i = 0; i < m.Overloads.GetCount(); i++) {
			if (m.Overloads[i].IsScanned == false)
				BuildSignature(conCls, m.Overloads[i]);
			if (m.Overloads[i].IsScanned == true)
				goodOvers++;
		}
		
		if (goodOvers != m.Overloads.GetCount())
			ErrorReporter::SomeOverloadsBad(conCls.Name, p, s);
			
		OverloadResolver res(ass);
		Overload* found = res.Resolve(m, params, 0);
		
		if (res.IsAmbig())
			ErrorReporter::AmbigError(conCls.Name, p, ass, &m.OwnerClass, &m, params, res.Score());

		if (!found)
			ErrorReporter::CantCall(conCls.Name, p, ass, &m.OwnerClass, &m, params, 2);
		
		if (conOver)
			conOver->DepOver.FindAdd(found);
		
		CallNode* call = irg.call(*found);
		FixupParams(*found, params);
		call->Params = pick(params);
		
		return call;
	}
	
	i = conCls.Variables.Find(s);
	if (i != -1) {
		parser.WSCurrentLine();
		
		if (!conCls.Variables[i].IsEvaluated)
			ErrorReporter::UndeclaredIdentifier(conCls.Name, p, s);
		
		return irg.mem(conCls.Variables[i]);
	}
	
	ZClass* c = GetClass(s);
	if (!c)
		ErrorReporter::UndeclaredIdentifier(conCls.Name, p, s);
	
	return irg.constClass(c);
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
		exp = irg.constIntSigned(oInt, base, ass.CSmall);
	else if (type == ZParser::ntShort)
		exp = irg.constIntSigned(oInt, base, ass.CShort);
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

Node* Compiler::ParseTemporary(ZClass& conCls, Overload* conOver, ZParser& parser, const Point p, ZClass& cls) {
	if (parser.Char('}'))
		return GetVarDefault(&cls);
	else {
		Node* exp = ParseExpression(conCls, conOver, parser);
		parser.WS();
		parser.Expect('}');
		parser.WSCurrentLine();
		parser.OpenCB--;
		
		return irg.cast(exp, &cls);
	}
}

Node* Compiler::ParseDot(ZClass& conCls, Overload* conOver, ZParser& parser, Node* exp) {
	Point p = parser.GetPoint();
	String s;
	
	s = parser.ExpectId();
	
	if (s == CLS_STR) {
		if (exp->Class == ass.CCls) {
			if (!exp->IsLiteral)
				ErrorReporter::Warning(conCls.Name, p, "chained .class statements detected. Expression is equivalent to 'Class.class' which is 'Class'");
			if (exp->IntVal == ass.CCls->MIndex)
				ErrorReporter::Warning(conCls.Name, p, "'Class.class' is 'Class'. Is this intentional?");
			
			exp = irg.constClass(&ass.Classes[(int)exp->IntVal]);
			exp->IsLiteral = false;

			ASSERT(exp->Class);
			
			return exp;
		}
		else {
			exp = irg.constClass(exp->Class, exp);
			exp->IsLiteral = false;
			
			ASSERT(exp->Class);
			
			return exp;
		}
	}
	
	ZClass* cobj = exp->Class;
	if (cobj == ass.CCls)
		cobj = &ass.Classes[(int)exp->IntVal];
	
	if (conOver)
		conOver->DepClass.FindAdd(cobj);
	
	if (!cobj->IsEvaluated)
		CompileClass(*cobj);
	
	int i = cobj->Variables.Find(s);
	if (i != -1) {
		Variable& v = cobj->Variables[i];
		
		/*if (exp == nullptr) {
			if (InConstMode)
				parser.Error(p, "'" + s + "' is not a constant, can't use it to initialize a constant");
			if (!v.IsStatic && over && over->IsStatic)
				parser.Error(p, "'" + s + "' is not a static, can't access it in a static function");
			if (InVarMode && !v.IsDeclared)
				parser.Error(p, "class member initialization must be done in order, use of '" + v.Name +
						"' before it was declared at (" + IntStr(v.Location.x) + ", " + IntStr(v.Location.y) + ")");
		}
		else {*/
			/*if (v.Access != Variable::atPublic) {
				if (&conCls != exp->Class)
					ErrorReporter::CantAccess(*cls->Source, p, v);
			}*/
		//}
		
		v.InUse = true;
		
		MemNode* node = irg.mem(cobj->Variables[i]);
		//node->IsClass = true;
		
		return node;
	}
	
	ErrorReporter::UndeclaredIdentifier(conCls.Name, p, ass.Classes[(int)exp->IntVal].Name, s);
	
	return nullptr;
}

void Compiler::GetParams(Vector<Node*>& params, ZClass& conCls, Overload* conOver, ZParser& parser, char end) {
	while (!parser.IsChar(end)) {
		params.Add(CompileExpression(conCls, conOver, parser));
		
		if (parser.IsChar(end))
		    break;
		
		if (parser.IsChar(',')) {
			parser.Char(',');
			parser.WS();
		}
	}
	
	parser.Expect(end);
	parser.WSCurrentLine();
}

void Compiler::FixupParams(Overload& over, Vector<Node*>& params) {
	int c = min(over.Params.GetCount(), params.GetCount());
	int d = max(over.Params.GetCount(), params.GetCount());
	
	for (int i = 0; i < c; i++) {
		if (over.Params[i].Class->MIsNumeric && over.Params[i].Class != params[i]->Class)
			params[i] = irg.cast(params[i], over.Params[i].Class);
	}
	
	for (int i = c; i < d; i++) {
		ASSERT(over.Params[i].Default);
		
		if (over.Params[i].Class->MIsNumeric && over.Params[i].Class != over.Params[i].Default->Class)
			params << irg.cast(over.Params[i].Default, over.Params[i].Class);
		else
			params << over.Params[i].Default;
	}
	
	ASSERT(params.GetCount() == over.Params.GetCount());
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
	
	if (parser.IsChar2('<', '<'))
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
