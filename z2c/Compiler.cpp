#include "Compiler.h"
#include "ErrorReporter.h"

namespace Z2 {

bool Compiler::CompileSnip(const String& snip) {
	String temp = snip + "}";
	ZParser tempParser(temp);
	
	ZClass tempClass;
	tempClass.Name = "DummyClass";
	tempClass.BackendName = "DummyClass";
	
	Overload& tempOver = tempClass.AddOverload();
	tempOver.Name = "@main";
	tempOver.EntryPoint = tempParser.GetPos();
	
	return CompilerOverload(tempOver);
}

bool Compiler::CompilerOverload(Overload& conOver) {
	ZParser parser;
	parser.Path = conOver.OwnerClass.Name;
	
	parser.SetPos(conOver.EntryPoint);

	CompileBlock(conOver.OwnerClass, conOver, parser, 1);
	
	cpp.ResetIndent();
	
	for (int i = 0; i < conOver.Nodes.GetCount(); i++)
		cpp.WalkStatement(conOver.Nodes[i]);
	
	return true;
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
	
	return valid;
}

Node* Compiler::CompileVar(ZClass& conCls, Overload& conOver, ZParser& parser) {
	Point p = parser.GetPoint();
	parser.WS();
			
	String varName = parser.ExpectId();
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

