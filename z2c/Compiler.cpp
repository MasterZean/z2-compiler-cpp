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
	
	return true;
}

bool Compiler::CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level) {
	bool valid = true;
	
	conOver.Blocks.Add();
	conOver.Blocks.Top().Temps = 0;
	
	while (!parser.IsChar('}')) {
		if (!CompileStatement(conCls, conOver, parser))
			valid = false;
	}
	
	conOver.Blocks.Drop();
	
	return valid;
}

bool Compiler::CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser) {
	bool valid = true;
	
	Node* exp = nullptr;
	
	try {
		if (parser.Id("val"))
			exp = CompileVar(conCls, conOver, parser);
		else {
			exp = Parse(conCls, &conOver, parser);
			parser.ExpectEndStat();
			parser.OS();
		}
		
		cpp.Walk(exp);
		cpp.ES();
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
	parser.OS();
			
	String varName = parser.ExpectId();
	parser.OS();
			
	parser.Expect('=');
	parser.OS();
			
	Node* value = Parse(conCls, &conOver, parser);
	parser.ExpectEndStat();
	parser.OS();
	
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

Node* Compiler::Parse(ZClass& conCls, Overload* conOver, ZParser& parser) {
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

