#include "Compiler.h"
#include "ErrorReporter.h"

namespace Z2 {

extern char tab1[];
extern char tab2[];
extern char tab3[];

Overload* Compiler::CompileSnip(const String& snip) {
	String temp = snip + "}";
	ZParser tempParser(temp);
	
	ZClass& tempClass = ass.AddClass();
	tempClass.Name = "DummyClass";
	tempClass.BackendName = "DummyClass";
	
	Overload& tempOver = tempClass.AddOverload();
	tempOver.Name = "@main";
	tempOver.BackendName = "_main";
	tempOver.EntryPoint = tempParser.GetPos();
	tempOver.Return = ass.CVoid;
	
	CompilerOverload(tempOver);
	
	for (int i = 0; i < postOverloads.GetCount(); i++)
		CompilerOverload(*postOverloads[i]);
	
	return &tempOver;
}

bool Compiler::CompilerOverload(Overload& overload) {
	ZParser parser;
	parser.Path = overload.OwnerClass.Name;
	
	parser.SetPos(overload.EntryPoint);

	CompileBlock(overload.OwnerClass, overload, parser, 1);
	
	return true;
}

void Compiler::WriteOverloadBody(Overload& overload, int indent) {
	//ss.Create();
	
	cpp.ResetIndent(indent);
	
	for (int i = 0; i < overload.Nodes.GetCount(); i++)
		cpp.WalkStatement(overload.Nodes[i]);
}

void Compiler::WriteOverload(Overload& overload) {
	ss.Create();
	cpp.WriteOverloadDefinition(overload);
	WriteOverloadBody(overload, 1);
	ss << "}\r\n\r\n";
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
			
			Overload& tempOver = conCls.AddOverload();
			tempOver.Name = name;
			tempOver.BackendName = name;
			tempOver.EntryPoint = parser.GetPos();
			tempOver.Return = ass.CVoid;
			
			postOverloads.Add(&tempOver);
			
			ScanBlock(conCls, parser);
			
			return true;
		}
		else
			exp = ParseExpression(conCls, &conOver, parser);
			
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
		
		parser.SkipError();
		parser.Spaces();
	}
	catch (Exc& err) {
		Cout() << err;
	}
	
	return valid;
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
	//else if (parser.IsCharConst())
	//	parser.ReadChar();
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
	Point p = parser.GetPoint();
	parser.WS();
			
	String varName = parser.ExpectZId();
	parser.WS();
			
	parser.Expect('=');
	parser.WS();
			
	Node* value = ParseExpression(conCls, &conOver, parser);
	
	if (conOver.Name == varName)
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
	
	Variable& v = conOver.AddVariable();
	v.Name = varName;
	v.SourcePos = p;
	v.Value = value;
		
	conOver.Blocks.Top().AddVaribleRef(v);
	
	return irg.defineLocalVar(v);
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

