#include "Compiler.h"
#include "ErrorReporter.h"
#include "tables.h"

namespace Z2 {

Overload* Compiler::CompileSnipFunc(const String& snip) {
	ZClass& tempClass = ass.AddClass("DummyClass");
	tempClass.BackendName = "DummyClass";
	
	String temp = snip + "}";
	ZParser tempParser(temp);
	tempParser.Path = tempClass.Name;
	
	Method& main = tempClass.GetAddMethod("@main");
	main.BackendName = "_main";
	
	Overload& tempOver = main.AddOverload();
	tempOver.EntryPos = tempParser.GetPos();
	tempOver.Return = ass.CVoid;
	
	tempParser.SetPos(tempOver.EntryPos);
	
	CompileOverload(tempOver, tempParser);
	
	return &tempOver;
}

ZClass* Compiler::CompileAnonClass(const String& snip) {
	ZClass& tempClass = ass.AddClass("DummyClass");
	tempClass.BackendName = "DummyClass";
	
	ZParser scan(snip);
	Scan(tempClass, scan);
	
	ZParser tempParser(snip);
	tempParser.Path = tempClass.Name;
	
	CompileSourceLoop(tempClass, tempParser);
	
	return &tempClass;
}

bool Compiler::CompileSourceLoop(ZClass& conCls, ZParser& parser) {
	bool valid = true;
	
	while (!parser.IsChar('}')) {
		try {
			int line = parser.GetLine();
			bool checkEnd = true;
			
			if (parser.Id("val"))
				CompileVar(conCls, nullptr, parser, false);
			else if (parser.Id("const"))
				CompileVar(conCls, nullptr, parser, true);
			else if (parser.Id("class")) {
				parser.WSCurrentLine();
				
				Point p = parser.GetPoint();
				String name = parser.ExpectZId();
				parser.WSCurrentLine();
				
				ZClass* newClass = GetClass(conCls, p, name);
				
				parser.Expect('{');
				parser.WS();
				
				CompileSourceLoop(*newClass, parser);
			}
			else if (parser.Id("def") || parser.Id("func")) {
				parser.WSCurrentLine();
				
				Point p = parser.GetPoint();
				String name = parser.ExpectZId();
				
				int i = conCls.Methods.Find(name);
				ASSERT(i != -1);
				
				Method& m = conCls.Methods[i];
				Overload& over = m.GetOverloadByPoint(p);
				BuildSignature(conCls, over, parser);
				
				Vector<Overload*> jumps;
				
				while (parser.Char(',')) {
					parser.WS();
					
					if (parser.Id("def") || parser.Id("func")) {
						parser.WSCurrentLine();
						
						Point p = parser.GetPoint();
						String name = parser.ExpectZId();
						
						Overload& subover = m.GetOverloadByPoint(p);
						jumps << &subover;

						BuildSignature(conCls, subover, parser);
					}
				}
				
				parser.Expect('{');
				parser.WS();
								
				over.EntryPos = parser.GetPos();

				postOverloads.Add(&over);
				
				CompileOverload(over, parser);
				
				checkEnd = false;
				
				for (int i = 0; i < jumps.GetCount(); i++) {
					jumps[i]->EntryPos = over.EntryPos;
					CompileOverloadJump(*jumps[i]);
				}
			}
			else if (parser.Id("namespace")) {
				parser.WSCurrentLine();
				
				String total = parser.ExpectZId();
				total << ".";
	
				while (parser.Char('.')) {
					parser.WSCurrentLine();
					total << parser.ExpectZId() << ".";
					parser.WSCurrentLine();
				}
	
				parser.ExpectEndStat();
				
				conCls.Namespace = total;
			}
			else if (parser.IsEof()) {
				return valid;
			}
			else {
				Point p = parser.GetPoint();
				ErrorReporter::SyntaxError(conCls.Name, p, parser.Identify());
			}
			
			if (checkEnd) {
				// end statement
				if (parser.PeekChar() != '\r' && parser.PeekChar() != '\n')
					parser.WSCurrentLine();
				parser.ExpectEndStat();
				parser.WS();
			}
		}
		catch (ZSyntaxError& err) {
			if (compileStack.GetCount())
				err.Context = compileStack.Top();
			
			errors.Add(err);
			if (PrintErrors)
				err.PrettyPrint(Cout());
			valid = false;
			
			int line = parser.GetLine();
			while (true) {
				if (parser.IsChar('}')) {
					if (parser.OpenCB) {
						parser.Char('}');
						parser.Spaces();
						parser.OpenCB--;
					}
					else
						break;
				}
				else if (parser.Char(';')) {
					parser.Spaces();
					break;
				}
				else {
					parser.SkipError();
					parser.Spaces();
				}
				
				if (parser.GetLine() != line) {
					parser.Spaces();
					break;
				}
			}
		}
		catch (Exc& err) {
			Cout() << err;
		}
	}
	
	return valid;
}

bool Compiler::CompileOverload(Overload& overload, ZParser& parser) {
	compileStack << &overload;
	
	auto res = CompileBlock(overload.OwnerClass, overload, parser, &overload.Nodes, 1);
	
	compileStack.Pop();
	
	return res;
}

bool Compiler::CompileOverloadJump(Overload& overload) {
	compileStack << &overload;
	
	ZParser parser;
	parser.SetPos(overload.EntryPos);
	
	auto res = CompileBlock(overload.OwnerClass, overload, parser, &overload.Nodes, 1);
	
	compileStack.Pop();
	
	return res;
}

bool Compiler::CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, Vector<Node*>* nodePool, int level) {
	bool valid = true;
	
	parser.WS();
	
	conOver.Blocks.Add();
	conOver.Blocks.Top().Temps = 0;
	
	if (nodePool && level > 1)
		*nodePool << irg.openBlock();
	
	while (!parser.IsChar('}')) {
		if (parser.Char('{')) {
			if (!CompileBlock(conCls, conOver, parser, nodePool, level + 1))
				valid = false;
		}
		else {
			if (!CompileStatement(conCls, conOver, parser, nodePool))
				valid = false;
		}
	}
	
	if (nodePool && level > 1)
		*nodePool << irg.closeBlock();
	
	parser.Expect('}');
	parser.WS();
	
	conOver.Blocks.Drop();
	
	return valid;
}

bool Compiler::CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser, Vector<Node*>* nodePool) {
	bool valid = true;
	
	Node* exp = nullptr;
	
	try {
		if (conOver.Blocks.Top().Returned)
			ErrorReporter::UnreachableCode(conCls.Name, parser.GetPoint());
		
		int line = parser.GetLine();
		
		bool returned = false;
		bool expectES = true;
		
		if (parser.Id("val"))
			exp = CompileVar(conCls, &conOver, parser, false);
		else if (parser.Id("const"))
			exp = CompileVar(conCls, &conOver, parser, true);
		else if (parser.Id("return")) {
			if (conOver.Return == ass.CVoid)
				exp = irg.ret();
			else {
				parser.WSCurrentLine();
				Point ptRet = parser.GetPoint();
				Node* retVal = CompileExpression(conCls, &conOver, parser);
				exp = irg.ret(retVal);
				
				if (!ass.CanAssign(conOver.Return, retVal))
					ErrorReporter::CantAssign(conCls.Name, ptRet, conOver.Return->Name, retVal->Class->Name);
			}
			
			returned = true;
		}
		else if (parser.Id("if")) {
			exp = CompileIfWhile(conCls, &conOver, parser, nodePool, true);
			expectES = false;
		}
		else if (parser.Id("while")) {
			exp = CompileIfWhile(conCls, &conOver, parser, nodePool, false);
			expectES = false;
		}
		else {
			exp = CompileExpression(conCls, &conOver, parser);
			parser.WSCurrentLine();
			
			Point ptEq = parser.GetPoint();
			
			if (parser.Char('=')) {
				parser.WSCurrentLine();
				
				Node* rs = CompileExpression(conCls, &conOver, parser);
				
				if (!ass.CanAssign(exp->Class, rs))
					ErrorReporter::CantAssign(conCls.Name, ptEq, exp->Class->Name, rs->Class->Name);
				if (!exp->IsAddressable)
					ErrorReporter::AssignNotLValue(conCls.Name, ptEq);
				if (exp->IsReadOnly)
					ErrorReporter::AssignConst(conCls.Name, ptEq, exp->Class->Name);
				
				exp = irg.assign(exp, rs);
			}
			else if (parser.Char2('+', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opAdd);
			}
			else if (parser.Char2('-', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opSub);
			}
			else if (parser.Char2('*', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opMul);
			}
			else if (parser.Char2('/', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opDiv);
			}
			else if (parser.Char2('%', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opMod);
			}
			else if (parser.Char3('<', '<', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opShl);
			}
			else if (parser.Char3('>', '>', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opShr);
			}
			else if (parser.Char2('&', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opBitAnd);
			}
			else if (parser.Char2('^', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opBitXor);
			}
			else if (parser.Char2('|', '=')) {
				parser.WSCurrentLine();
				Node* rs = CompileExpression(conCls, &conOver, parser);
				exp = AssignOp(conCls, conOver, ptEq, exp, rs, OpNode::opBitOr);
			}
		}
			
		// end statement
		if (expectES) {
			if (parser.PeekChar() != '\r' && parser.PeekChar() != '\n')
				parser.WSCurrentLine();
			parser.ExpectEndStat();
			parser.WS();
			
			if (nodePool && conOver.Blocks.Top().Returned == false)
				*nodePool << exp;
		}
		
		ASSERT(exp);
		exp->OriginalLine = line;
		
		conOver.Blocks.Top().Returned = returned;
	}
	catch (ZSyntaxError& err) {
		if (compileStack.GetCount())
			err.Context = compileStack.Top();
			
		errors.Add(err);
		if (PrintErrors)
			err.PrettyPrint(Cout());
		valid = false;
		
		SkipUntilNL(parser);
	}
	catch (Exc& err) {
		if (PrintErrors)
			Cout() << err;
		valid = false;
	}
	
	return valid;
}

bool Compiler::SkipUntilNL(ZParser& parser, bool cb) {
	int line = parser.GetLine();
	
	while (true) {
		if (parser.IsChar('}')) {
			if (parser.OpenCB) {
				parser.Char('}');
				parser.Spaces();
				parser.OpenCB--;
			}
			else
				return false;
		}
		else if (parser.Char(';')) {
			parser.Spaces();
			break;
		}
		else if (parser.Char('{')) {
			parser.Spaces();
			if (cb)
				return true;
		}
		else {
			parser.SkipError();
			parser.Spaces();
		}
		
		if (parser.GetLine() != line) {
			parser.Spaces();
			return false;
		}
	}
	
	return false;
}

Node* Compiler::AssignOp(ZClass& conCls, Overload& conDef, Point p, Node* exp, Node* rs, OpNode::Type op) {
	Node* test = irg.op(exp, rs, op, p);
	if (!test)
		ErrorReporter::IncompatOperands(conCls.Name, p, TabOpString[op], exp->Class->Name, rs->Class->Name);
	
	if (!ass.CanAssign(exp->Class, rs))
		ErrorReporter::CantAssign(conCls.Name, p, exp->Class->Name, rs->Class->Name);
	if (!exp->IsAddressable)
		ErrorReporter::AssignNotLValue(conCls.Name, p);
	if (exp->IsReadOnly)
		ErrorReporter::AssignConst(conCls.Name, p, exp->Class->Name);
	
	AssignNode* node = irg.assign(exp, rs);
	node->Op = op;
	
	return node;
}

Node* Compiler::CompileExpression(ZClass& conCls, Overload* conOver, ZParser& parser) {
	parser.OpenCB = 0;
	return ParseExpression(conCls, conOver, parser);
}

void Compiler::CompileClass(ZClass& cls) {
	if (cls.IsEvaluated)
		return;
	
	cls.IsEvaluated = true;
	
	ZParser parser;
	parser.SetPos(cls.EntryPos);
	parser.Path = cls.Name;
	
	CompileSourceLoop(cls, parser);
}

void Compiler::CompileVar(Variable& v) {
	if (!v.IsEvaluated)
		return;
	
	v.IsEvaluated = true;
}

Node* Compiler::CompileVar(ZClass& conCls, Overload* conOver, ZParser& parser, bool cst) {
	parser.WS();
	
	Point ptName = parser.GetPoint();
	
	String varName = parser.ExpectZId();
	parser.WS();
	
	ZClass* varClass = nullptr;
	Node* value = nullptr;
	
	Point ptEqual;
	
	if (parser.Char(':')) {
		parser.WS();
		
		Point tp = parser.GetPoint();
		varClass = GetClass(conCls, tp, parser.ExpectZId());
		parser.WSCurrentLine();
		
		Point temp = parser.GetPoint();
		if (parser.Char('=')) {
			parser.WS();
			
			ptEqual = temp;
			
			value = CompileExpression(conCls, conOver, parser);
		}
	}
	else {
		ptEqual = parser.GetPoint();
		parser.Expect('=');
		parser.WS();
		
		value = CompileExpression(conCls, conOver, parser);
		varClass = value->Class;
	}
	
	CheckLocalVar(conCls, conOver, varName, ptName);
	
	if (varClass == ass.CCls)
		ErrorReporter::CantCreateClassVar(conCls.Name, ptName, varClass->Name);
	if (varClass == ass.CVoid)
		ErrorReporter::CantCreateClassVar(conCls.Name, ptName, varClass->Name);
	
	if (varClass && value) {
		if (!ass.CanAssign(varClass, value))
			ErrorReporter::CantAssign(conCls.Name, ptEqual, varClass->Name, value->Class->Name);
	}
	else if (!value) {
		ASSERT(varClass);
		value = GetVarDefault(varClass);
	}
	ASSERT(value);
	
	if (varClass != value->Class)
		value = irg.cast(value, varClass);
	
	Variable& v = conOver ? conOver->AddVariable() : conCls.AddVariable(varName);
	v.Name = varName;
	v.SourcePoint = ptName;
	v.Value = value;
	v.Class = varClass;
	v.MIsMember = conOver == nullptr;
	v.IsReadOnly = cst;
	
	if (conOver == nullptr)
		v.OwnerClass = &conCls;
	
	if (conOver) {
		conOver->Blocks.Top().AddVaribleRef(v);
		
		return irg.defineLocalVar(v);
	}
	else
		return irg.defineLocalVar(v);
}

Node* Compiler::CompileIfWhile(ZClass& conCls, Overload* conOver, ZParser& parser, Vector<Node*>* nodePool, bool isIf) {
	Node* cond = nullptr;
	bool ocb = false;
		
	try {
		parser.WS();
		parser.Expect('(');
		parser.WS();
		
		Point p = parser.GetPoint();
		cond = CompileExpression(conCls, conOver, parser);
		
		parser.Expect(')');
		parser.WS();
		
		if (cond->Class != ass.CBool)
			ErrorReporter::CondNotBool(conCls.Name, p, cond->Class->Name);
	}
	catch (ZSyntaxError& err) {
		if (compileStack.GetCount())
			err.Context = compileStack.Top();
			
		errors.Add(err);
		if (PrintErrors)
			err.PrettyPrint(Cout());
		
		ocb = SkipUntilNL(parser, true);
		//valid = false;
	}
	catch (Exc& err) {
		if (PrintErrors)
			Cout() << err;
		//valid = false;
	}
	
	IfNode* ifNode = isIf ?
						irg.ifNode(cond ? cond : irg.constBool(false)) :
						irg.whileNode(cond ? cond : irg.constBool(false));
	
	Vector<Node*>* csPool = nodePool;
	if (cond == nullptr || (cond->IsCT && cond->IntVal == 0))
		csPool = nullptr;
	
	int origCount = 0;
	
	if (csPool) {
		origCount = nodePool->GetCount();
		*csPool << ifNode;
		*csPool << irg.openBlock(false);
	}
	
	if (parser.Char('{') || ocb)
		CompileBlock(conCls, *conOver, parser, csPool, 1);
	else
		CompileStatement(conCls, *conOver, parser, csPool);

	if (csPool) {
		*csPool << irg.closeBlock();
		if (isIf == false)
			*csPool << irg.gotoNode(origCount);
		ifNode->JumpOnFalse = nodePool->GetCount() - 1;
	}
	
	return cond;
}

Node* Compiler::GetVarDefault(ZClass* cls) {
	if (cls == ass.CBool)
		return irg.constBool(0);
	else if (cls->MIsNumeric)
		return irg.constIntSigned(0, cls);
	/*else if (ass.IsPtr(c->I.Tt))
		return const_null();
	else {
		ZClass* cls = c->I.Tt.Class;
		for (int i = 0; i < cls->Cons.GetCount(); i++)
			for (int j = 0; j < cls->Cons[i].Overloads.GetCount(); j++) {
				Overload& ol = cls->Cons[i].Overloads[j];
				if (ol.IsCons == 1 && ol.Params.GetCount() == 0)
					c->Body = mem_temp(cls->Tt, &ol);
			}
	}*/
	
	return nullptr;
}

void Compiler::CheckLocalVar(ZClass& conCls, Overload* conOver, const String& varName, const Point& p) {
	if (conCls.Name == varName)
		ErrorReporter::Dup(conCls.Name, p, conOver->SourcePoint, varName);
		
	for (int i = 0; i < conCls.Variables.GetCount(); i++)
		if (conCls.Variables[i].Name == varName)
			ErrorReporter::Warning(conCls.Name, p, "local '" + varName + "' hides a class member");
		
	if (conOver) {
		if (conOver->OwnerMethod.Name == varName)
			ErrorReporter::Dup(conCls.Name, p, conOver->SourcePoint, varName);
		
		for (int i = 0; i < conOver->Params.GetCount(); i++)
			if (conOver->Params[i].Name == varName)
				ErrorReporter::Dup(conCls.Name, p, conOver->Params[i].SourcePoint, varName);
	
		for (int j = 0; j < conOver->Blocks.GetCount(); j++)
			for (int k = 0; k < conOver->Blocks[j].Variables.GetCount(); k++) {
				if (conOver->Blocks[j].Variables[k]->Name == varName)
					ErrorReporter::Dup(conCls.Name, p, conOver->Blocks[j].Variables[k]->SourcePoint, varName);
			}
	}
}

void Compiler::BuildSignature(ZClass& conCls, Overload& over) {
	ZParser parser;
	parser.SetPos(over.ParamPos);
	parser.Path = over.OwnerClass.Name;
	
	compileStack << nullptr;
	
	try {
		BuildSignature(conCls, over, parser);
	}
	catch (ZSyntaxError& err) {
		if (compileStack.GetCount())
			err.Context = compileStack.Top();
			
		errors.Add(err);
		if (PrintErrors)
			err.PrettyPrint(Cout());
		//valid = false;
	}
	catch (Exc& err) {
		if (PrintErrors)
			Cout() << err;
		//valid = false;
	}
	
	compileStack.Pop();
}

void Compiler::BuildSignature(ZClass& conCls, Overload& over, ZParser& parser) {
	if (over.IsScanned) {
		parser.SetPos(over.PostParamPos);
		return;
	}
	
	parser.WSCurrentLine();
	parser.Expect('(');
	parser.WSCurrentLine();
	
	int count = 0;
	
	while (!parser.IsChar(')')) {
		bool move = false;
		
		if (parser.Id("move")) {
			parser.WS();
			move = true;
		}
		
		String pname = parser.ExpectZId();
		parser.WSCurrentLine();
		parser.Expect(':');
		parser.WSCurrentLine();
		
		Point tp = parser.GetPoint();
		ZClass* pcls = GetClass(conCls, tp, parser.ExpectZId());
		parser.WSCurrentLine();
		
		Variable& var = over.Params.Add(pname);
		var.MIsParam = count;
		var.Name = pname;
		var.Class = pcls;
		
		if (parser.Char('=')) {
			parser.WSCurrentLine();
			
			Point p = parser.GetPoint();
			var.Default = CompileExpression(conCls, &over, parser);
			
			if (!var.Default->IsCT)
				ErrorReporter::ExpectCT(conCls.Name, p);
			
			if (over.MinParams == -1)
				over.MinParams = count;
		}
		
		if (move)
			var.PType = Variable::tyMove;
		else
			var.PType = Variable::tyAuto;
		
		if (var.PType == Variable::tyAuto || var.PType == Variable::tyConstRef)
			var.IsReadOnly = true;
		else
			var.IsReadOnly = false;
		
		if (over.Signature.GetCount())
			over.Signature << ", ";
		if (move)
			over.Signature << "move ";
		over.Signature << pcls->Name;
		
		if (over.LogSig.GetCount())
			over.LogSig << ", ";
		if (move)
			over.LogSig << "move ";
		over.LogSig << pcls->Name;
		
		if (over.ParamSig.GetCount())
			over.ParamSig << ", ";
		over.ParamSig << pcls->ParamName;
		if (move)
			over.ParamSig << "&&";
		
		count++;
		
		if (parser.IsChar(')'))
			break;
		
		if (parser.IsChar(',')) {
			parser.Char(',');
			parser.WS();
		}
	}
	
	if (over.MinParams == -1)
		over.MinParams = count;
	
	parser.Expect(')');
	parser.WSCurrentLine();
	
	LOG((over.IsConst ? "def " : "func ") + over.OwnerMethod.Name + "(" + over.Signature + ")" + " | " +
	    (over.IsConst ? "def " : "func ") + over.OwnerMethod.BackendName + "(" + over.ParamSig + ")");
	
	ZClass* ret = ass.CVoid;
	if (parser.Char(':')) {
		parser.WSCurrentLine();
		ret = GetClass(conCls, parser.GetPoint(), parser.ExpectZId());
		parser.WSCurrentLine();
	}
	
	over.PostParamPos = parser.GetPos();
	over.Return = ret;
	over.IsScanned = true;
	
	ASSERT(over.Return);
}

ZClass* Compiler::GetClass(ZClass& conCls, const Point& p, const String& name) {
	int i = ass.Classes.Find(name);
	
	if (i != -1)
		return &ass.Classes[i];
	
	ErrorReporter::UndeclaredClass(conCls.Name, p, name);
	
	return nullptr;
}

ZClass* Compiler::GetClass(const String& name) {
	int i = ass.Classes.Find(name);
	
	if (i != -1)
		return &ass.Classes[i];
	else
		return nullptr;
}

String Compiler::GetErrors() {
	String result;
	
	for (int i = 0; i < errors.GetCount(); i++) {
		result << errors[i].Path;
		result << "(" << errors[i].ErrorPoint.x << ", " << errors[i].ErrorPoint.y << ")";
		result << ": ";
		result << "error:\r\n\t";
		
		Vector<String> v = Split(errors[i].Error, '\f', false);
		
		for (int j = 0; j < v.GetCount(); j++)
			result << v[j];

		result << "\r\n";
	}
	
	return result;
}

void Compiler::Scan(ZClass& conCls, ZParser& parser) {
	// TODO: rewrite for performance
	
	while (!parser.IsEof()) {
		if (parser.Id("def"))
			ScanDef(conCls, parser, false);
		else if (parser.Id("func"))
			ScanDef(conCls, parser, true);
		else if (parser.Id("class")) {
			parser.WSCurrentLine();
			if (parser.IsZId()) {
				String name = parser.ReadZId();
				parser.WSCurrentLine();
				
				ZClass& newClass = ass.AddClass(name);
				newClass.BackendName = name;
				
				if (parser.Char('{')) {
					parser.WS();
					
					newClass.EntryPos = parser.GetPos();
					
					ScanClass(newClass, parser);
				}
				else
					ScanToken(parser);
			}
			else
				ScanToken(parser);
		}
		else
			ScanToken(parser);
	}
}

void Compiler::Scan(ZParser& parser) {
}
	
void Compiler::ScanClass(ZClass& conCls, ZParser& parser) {
	int o = 1;
	int c = 0;
	
	while (!parser.IsEof()) {
		if (parser.Id("def"))
			ScanDef(conCls, parser, false);
		else if (parser.Id("func"))
			ScanDef(conCls, parser, true);
		else if (parser.Char('{')) {
			parser.WS();
			o++;
		}
		else if (parser.Char('}')) {
			parser.WS();
			c--;
			
			if (o == c)
				return;
		}
		else
			ScanToken(parser);
	}
}

void Compiler::ScanDef(ZClass& conCls, ZParser& parser, bool ct) {
	parser.Spaces();
			
	Point p = parser.GetPoint();
	
	if (parser.IsZId()) {
		String name = parser.ReadZId();
		
		Method& main = conCls.GetAddMethod(name);
		if (name[0] == '@') {
			main.BackendName = "_";
			main.BackendName << name.Mid(1);
		}
		else
			main.BackendName = name;
		
		Overload& over = main.AddOverload();
		over.ParamPos = parser.GetPos();
		over.NamePoint = p;
		over.IsConst = ct;
	}
	else
		ScanToken(parser);
}
	
// TODO: fix
void Compiler::ScanToken(ZParser& parser) {
	parser.WS();
	
	if (parser.IsInt()) {
		try {
			int64 oInt;
			double oDub;
			int base;
			parser.ReadInt64(oInt, oDub, base);
		}
		catch (ZSyntaxError& err) {
			parser.SkipError();
			parser.WS();
		}
	}
	else if (parser.IsString())
		parser.ReadString();
	else if (parser.IsZId())
		parser.ReadZId();
	else if (parser.IsId())
		parser.ReadId();
	else if (parser.IsCharConst())
		parser.ReadChar();
	else {
		for (int i = 0; i < 9; i++)
			if (parser.Char2(tab2[i], tab3[i])) {
				parser.Spaces();
			    return;
			}
		for (int i = 0; i < 24; i++)
			if (parser.Char(tab1[i])) {
				parser.Spaces();
			    return;
			}
		if (parser.Char('{') || parser.Char('}')) {
			parser.Spaces();
		    return;
		}
		DUMP(parser.Identify());
		ASSERT(0);
		//Point p = parser.GetPoint();
		//parser.Error(p, "syntax error: " + parser.Identify() + " found");
	}
	
	parser.WS();
}

bool Compiler::AddPackage(const String& aPath) {
	String pakName = GetFileName(NativePath(aPath));
	if (pakName.GetCount() == 0)
		pakName = GetFileName(GetFileFolder(aPath));
	
	FindFile ff(aPath);
	
	if (!ff.IsFolder()) {
		Cout() << "Could not find package '" << pakName << "' in folder '" << aPath << "'.\n";
		Cout() << "Exiting!\n";
		return false;
	}
	
	int pakIndex = packages.Find(pakName);
	if (pakIndex != -1) {
		ZPackage& existingPak = packages[pakIndex];
		
		if (existingPak.Path != aPath) {
			Cout() << "A package with the same name is already referenced at '" << existingPak.Path << "'.\n";
			Cout() << "Exiting!\n";
			return false;
		}
	}
	else
		pakIndex = packages.FindAdd(pakName);
	
	ZPackage& package = packages[pakIndex];
	package.Name = pakName;
	package.Path = AppendFileName(aPath, "");
	
	package.CachePath = NativePath(BuildPath + "\\" + package.Name);
	DirectoryCreate(package.CachePath);
	
	ZPackage temp;
	LoadFromFile(temp, NativePath(package.CachePath + "\\cache.dat"));
	
	AddModule(0, package.Path, package, temp);
	
	return true;
}

void Compiler::AddModule(int parent, const String& path, ZPackage& pak, ZPackage& temp) {
	FindFile ff;
	ff.Search(path + "/*");

	String post;
	
	while (ff) {
		if (ff.IsFile()) {
			String name = ff.GetName();

			if (GetFileExt(name) == ".z2")
				AddModuleSource(pak, temp, ff);
			else if (name.EndsWith(".msc.binds") && MSC)
				AddBindsFile(ff.GetPath());
			else if (name.EndsWith(".gcc.binds") && !MSC)
				AddBindsFile(ff.GetPath());
			else if (name.EndsWith("." + BMName + ".binds"))
				post = ff.GetPath();
			else if (!name.EndsWith(".msc.binds") && !name.EndsWith(".gcc.binds") && (name.EndsWith(".binds") && GetFileExt(GetFileExt(name)) == ""))
				AddBindsFile(ff.GetPath());
		}
		else if (ff.IsFolder())
			AddModule(parent + 1, ff.GetPath(), pak, temp);
		
		ff.Next();
	}
	
	if (post.GetCount())
		AddBindsFile(post);
}

void Compiler::AddModuleSource(ZPackage& pak, ZPackage& temp, FindFile& ff) {
	String filePath = ff.GetPath();
	String fileName = filePath.Mid(pak.Path.GetLength());
	int i = temp.Files.Find(fileName);
	
	if (i != -1) {
		Time t = FileGetTime(filePath);

		if (t.Compare(temp.Files[i].Modified)) {
			// "Overwriting file
			ZSource& zs = AddSource(pak, filePath);
		}
		else {
			// Skipping file
			ZSource& srcIn = temp.Files[i];
			ZSource& srcSkip = pak.Files.Add(fileName);
			
			srcSkip.Path = fileName;
			srcSkip.Modified = srcIn.Modified;
			srcSkip.Package = &pak;
			//srcSkip.ClassNameList = clone(srcIn.ClassNameList);
			
			/*for (int i = 0; i < srcSkip.ClassNameList.GetCount(); i++) {
				LookUp.Add(srcSkip.ClassNameList[i], filePath);
				
				int dd = srcSkip.ClassNameList[i].ReverseFind('.');
				String name = srcSkip.ClassNameList[i].Mid(dd + 1);
				ass.AddClassCount(name);
			}*/
		}
	}
	else {
		// "Adding file
		ZSource& zs = AddSource(pak, filePath);
	}
}

ZSource& Compiler::AddSource(ZPackage& aPackage, const String& aFile, bool populate) {
	String relPath = aFile.Mid(aPackage.Path.GetLength());
	int fileIndex = aPackage.Files.FindAdd(relPath);
	
	ZSource& source = aPackage.Files[fileIndex];
	source.Path = relPath;
	source.Modified = FileGetTime(aFile);
	source.Package = &aPackage;
	
	return LoadSource(source, populate);
}

ZSource& Compiler::LoadSource(ZSource& source, bool populate) {
	if (source.IsScaned)
		return source;
	
	String fullPath = source.Package->Path + source.Path;
	//Cout() << "Loading: " << fullPath << "\n";
	filesOpened++;
	
	if (Cache) {
		int i = Cache->Find(fullPath);
		if (i != -1)
			source.Data = (*Cache)[i];
		else
			source.Data = LoadFile(fullPath);
	}
	else
		source.Data = LoadFile(fullPath);
	
	/*Scanner scanner(source, Platform == WINDOWS32);
	scanner.Scan();
	source.AddStdClassRefs();
	source.IsScaned = true;
		
	if (populate) {
		ass.AddSource(source);
		Populate(source);
	}*/
	
	return source;
}

void Compiler::AddBindsFile(const String& path) {
	FileIn f(path);
	
	String key;
	String con;
	
	while (!f.IsEof()) {
		String line = f.GetLine();
		
		if (line.EndsWith(":")) {
			if (key.GetCount() && con.GetCount())
				Binds.FindAdd(key, con);
			
			key = line.Mid(0, line.GetCount() - 1);
			con = "";
		}
		else {
			if (con.GetCount())
				con << "\n";
			
			con << line;
		}
	}
	
	if (key.GetCount() && con.GetCount())
		Binds.FindAdd(key, con);
}

void Compiler::Sanitize(ZClass& cls) {
	for (int i = 0; i < cls.Methods.GetCount(); i++) {
		Method& m = cls.Methods[i];
		Sanitize(m);
	}
}

void Compiler::Sanitize(Method& m) {
	int index = 1;
		
	for (int i = 0; i < m.Overloads.GetCount(); i++) {
		Overload& o = m.Overloads[i];
		
		m.OverloadCounts.At(o.Params.GetCount())++;
		
		for (int j = 0; j < i; j++) {
			Overload& o2 = m.Overloads[j];
			
			if (/*o.BackSig == o2.BackSig || */o.ParamSig == o2.ParamSig) {
				o.BackendName << index;
				index++;
			}
		}
	}
}

}
