#include "Compiler.h"
#include "ErrorReporter.h"
#include "tables.h"

namespace Z2 {

Overload* Compiler::CompileSnip(const String& snip) {
	ZClass& tempClass = ass.AddClass("DummyClass");
	tempClass.BackendName = "DummyClass";
	
	ZParser scan(snip);
	Scan(tempClass, scan);
	
	String temp = snip + "}";
	ZParser tempParser(temp);
	
	Method& main = tempClass.GetAddMethod("@main");
	main.BackendName = "_main";
	
	Overload& tempOver = main.AddOverload();
	tempOver.EntryPoint = tempParser.GetPos();
	tempOver.Return = ass.CVoid;
	
	CompilerOverload(tempOver);
	
	for (int i = 0; i < postOverloads.GetCount(); i++)
		CompilerOverload(*postOverloads[i]);
	
	return &tempOver;
}

void Compiler::Scan(ZClass& conCls, ZParser& parser) {
	// TODO: rewrite for performance
	
	while (!parser.IsEof()) {
		if (parser.Id("def")) {
			parser.Spaces();
			
			Point p = parser.GetPoint();
			
			if (parser.IsId()) {
				String name = parser.ReadId();
				
				Method& main = conCls.GetAddMethod(name);
				main.BackendName = name;
				main.AddOverload();
			}
			else
				ScanToken(parser);
		}
		else
			ScanToken(parser);
	}
}

void Compiler::ScanToken(ZParser& parser) {
	parser.Spaces();
	
	if (parser.IsInt()) {
		try {
			int64 oInt;
			double oDub;
			int base;
			parser.ReadInt64(oInt, oDub, base);
		}
		catch (ZSyntaxError& err) {
			parser.SkipError();
			parser.Spaces();
		}
	}
	else if (parser.IsString())
		parser.ReadString();
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
	
	parser.Spaces();
}

bool Compiler::CompilerOverload(Overload& overload) {
	ZParser parser;
	parser.Path = overload.OwnerClass.Name;
	
	parser.SetPos(overload.EntryPoint);

	CompileBlock(overload.OwnerClass, overload, parser, 1);
	
	return true;
}

void Compiler::WriteOverloadBody(CppNodeWalker& cpp, Overload& overload, int indent) {
	cpp.ResetIndent(indent);
	
	for (int i = 0; i < overload.Nodes.GetCount(); i++)
		cpp.WalkStatement(overload.Nodes[i]);
}

void Compiler::WriteOverload(CppNodeWalker& cpp, Overload& overload) {
	cpp.WriteOverloadDefinition(overload);
	
	cpp.OpenOverload();
	WriteOverloadBody(cpp, overload, 1);
	cpp.CloseOverload();
}

bool Compiler::CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level) {
	bool valid = true;
	
	parser.WS();
	
	conOver.Blocks.Add();
	conOver.Blocks.Top().Temps = 0;
	
	if (level > 1)
		conOver.Nodes << irg.openBlock();
	
	while (!parser.IsChar('}')) {
		if (parser.Char('{')) {
			if (!CompileBlock(conCls, conOver, parser, level + 1))
				valid = false;
		}
		else {
			if (!CompileStatement(conCls, conOver, parser))
				valid = false;
		}
	}
	
	if (level > 1)
		conOver.Nodes << irg.closeBlock();
	
	parser.Expect('}');
	parser.WS();
	
	conOver.Blocks.Drop();
	
	return valid;
}

bool Compiler::CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser) {
	bool valid = true;
	
	Node* exp = nullptr;
	
	try {
		int line = parser.GetLine();
		
		if (parser.Id("val"))
			exp = CompileVar(conCls, conOver, parser);
		else if (parser.Id("def")) {
			parser.WS();
			String name = parser.ExpectZId();
			parser.WS();
			parser.Expect('(');
			parser.WS();
			parser.Expect(')');
			parser.WS();
			parser.Expect('{');
			parser.WS();
			
			int i = conCls.Methods.Find(name);
			ASSERT(i != -1);
			
			Overload& tempOver = conCls.Methods[i].Overloads[0];
			tempOver.EntryPoint = parser.GetPos();
			tempOver.Return = ass.CVoid;
			
			postOverloads.Add(&tempOver);
			
			ScanBlock(conCls, parser);
			
			return true;
		}
		else
			exp = CompileExpression(conCls, &conOver, parser);
			
		// end statement
		if (parser.PeekChar() != '\r' && parser.PeekChar() != '\n')
			parser.WSCurrentLine();
		parser.ExpectEndStat();
		parser.WS();
		
		exp->OriginalLine = line;
		
		conOver.Nodes << exp;
	}
	catch (ZSyntaxError& err) {
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
	
	return valid;
}

Node* Compiler::CompileExpression(ZClass& conCls, Overload* conOver, ZParser& parser) {
	parser.OpenCB = false;
	return ParseExpression(conCls, conOver, parser);
}

void Compiler::ScanBlock(ZClass& conCls, ZParser& parser) {
	while (!parser.IsChar('}')) {
		if (parser.Char('{')) {
			parser.WS();
		    ScanBlock(conCls, parser);
		}
		else
			ScanToken(conCls, parser);
	}
	
	parser.Expect('}');
	parser.WS();
}

void Compiler::ScanToken(ZClass& conCls, ZParser& parser) {
	parser.WS();
	
	if (parser.IsInt()) {
		int64 oInt;
		double oDub;
		int base;
		parser.ReadInt64(oInt, oDub, base);
	}
	else if (parser.IsString())
		parser.ReadString();
	else if (parser.IsZId())
		parser.ReadId();
	else if (parser.IsId())
		parser.ReadId();
	else if (parser.IsCharConst())
		parser.ReadChar();
	else {
		for (int i = 0; i < 9; i++)
			if (parser.Char2(tab2[i], tab3[i]))
			    return;
		for (int i = 0; i < 24; i++)
			if (parser.Char(tab1[i]))
			    return;
		if (parser.IsChar('{') || parser.IsChar('}'))
			return;

		Point p = parser.GetPoint();
		ErrorReporter::SyntaxError(conCls.Name, p, parser.Identify());
	}
}

Node* Compiler::CompileVar(ZClass& conCls, Overload& conOver, ZParser& parser) {
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
		parser.WS();
		
		Point temp = parser.GetPoint();
		if (parser.Char('=')) {
			parser.WS();
			
			ptEqual = temp;
			
			value = CompileExpression(conCls, &conOver, parser);
		}
	}
	else {
		ptEqual = parser.GetPoint();
		parser.Expect('=');
		parser.WS();
		
		value = CompileExpression(conCls, &conOver, parser);
		varClass = value->Class;
	}
	
	CheckLocalVar(conCls, conOver, varName, ptName);
	
	if (varClass == ass.CCls)
		ErrorReporter::CantCreateClassVar(conCls.Name, ptName, ass.CCls->Name);
	
	if (varClass && value) {
		if (!CanAssign(varClass, value))
			ErrorReporter::CantAssign(conCls.Name, ptEqual, varClass->Name, value->Class->Name);
	}
	else if (!value) {
		ASSERT(varClass);
		value = GetVarDefault(varClass);
	}
	ASSERT(value);
	
	if (varClass != value->Class)
		value = irg.cast(value, varClass);
	
	Variable& v = conOver.AddVariable();
	v.Name = varName;
	v.SourcePos = ptName;
	v.Value = value;
	v.Class = varClass;
		
	conOver.Blocks.Top().AddVaribleRef(v);
	
	return irg.defineLocalVar(v);
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

bool Compiler::CanAssign(ZClass* cls, Node* n) {
	if (cls == ass.CVoid || n->Class == ass.CVoid)
		return false;

	//if (ass.IsPtr(Tt))
	//	return y.Tt.Class == ass.CNull ||
	//		(ass.IsPtr(y.Tt) && (y.Tt.Next->Class == Tt.Next->Class || Tt.Next->Class == ass.CVoid));

	//if (isCt && Tt.Class == ass.CPtrSize && (y.Tt.Class == ass.CInt))
	//	return true;

	if (cls == n->Class)
		return true;
	
	if (cls->MIsNumeric && n->Class->MIsNumeric) {
		ASSERT(n->C1);
		int t1 = cls->MIndex;
		int t2 = n->C1->MIndex;
		ASSERT(t1 >= 0 && t1 <= 13);
		ASSERT(t2 >= 0 && t2 <= 13);
		
		if (TabCanAssign[t1][t2])
			return true;
		else {
			if (n->C2 != NULL) {
				t2 = n->C2->MIndex;
				return TabCanAssign[t1][t2];
			}
			else
				return false;
		}
	}

	return false;
	//return BaseExprParser::TypesEqualDeep(ass, &this->Tt, &y.Tt);
}

void Compiler::CheckLocalVar(ZClass& conCls, Overload& conOver, const String& varName, const Point& p) {
	if (conOver.OwnerMethod.Name == varName)
		ErrorReporter::Dup(conCls.Name, p, conOver.SourcePos, varName);
	if (conCls.Name == varName)
		ErrorReporter::Dup(conCls.Name, p, conOver.SourcePos, varName);

	for (int i = 0; i < conOver.Params.GetCount(); i++)
		if (conOver.Params[i].Name == varName)
			ErrorReporter::Dup(conCls.Name, p, conOver.Params[i].SourcePos, varName);

	for (int j = 0; j < conOver.Blocks.GetCount(); j++)
		for (int k = 0; k < conOver.Blocks[j].Variables.GetCount(); k++) {
			if (conOver.Blocks[j].Variables[k]->Name == varName)
				ErrorReporter::Dup(conCls.Name, p, conOver.Blocks[j].Variables[k]->SourcePos, varName);
		}
		
	for (int i = 0; i < conCls.Variables.GetCount(); i++)
		if (conCls.Variables[i].Name == varName)
			ErrorReporter::Warning(conCls.Name, p, "local '" + varName + "' hides a class member");
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
		result << ": ";
		result << "error:\r\n\t";
		
		Vector<String> v = Split(errors[i].Error, '\f', false);
		
		for (int j = 0; j < v.GetCount(); j++)
			result << v[j];

		result << "\r\n";
	}
	
	return result;
}

}
