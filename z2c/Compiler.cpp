#include "Compiler.h"
#include "ErrorReporter.h"

namespace Z2 {

bool Compiler::CompileSnip(const String& snip) {
	String temp = snip + "}";
	ZParser tempParser(temp);
	
	ZClass tempClass;
	tempClass.Name = "DummyClass";
	
	Overload& tempOver = tempClass.AddOverload();
	tempOver.Name = "@main";
	tempOver.EntryPoint = tempParser.GetPos();
	
	tempParser.Path = tempClass.Name;
	
	return CompilerOverload(tempOver);
}

bool Compiler::CompilerOverload(Overload& conOver) {
	ZParser parser;
	parser.Path = conOver.Class.Name;
	parser.SetPos(conOver.EntryPoint);

	CompileBlock(conOver.Class, conOver, parser, 1);
	
	return true;
}

bool Compiler::CompileBlock(ZClass& conCls, Overload& conOver, ZParser& parser, int level) {
	bool valid = true;
	
	while (!parser.IsChar('}')) {
		if (!CompileStatement(conCls, conOver, parser))
			valid = false;
	}
	
	return valid;
}

bool Compiler::CompileStatement(ZClass& conCls, Overload& conOver, ZParser& parser) {
	bool valid = true;
	
	Node* exp = nullptr;
	
	try {
		if (parser.IsInt()) {
			exp = ParseNumeric(conCls, parser);

			parser.ExpectEndStat();
			parser.Spaces();
		}
		else {
			Point p = parser.GetPoint();
			ErrorReporter::SyntaxError(conCls.Name, p, parser.Identify());
		}
		
		cpp.Walk(exp);
		ss << "\r\n";
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

Node* Compiler::ParseNumeric(ZClass& conCls, ZParser& parser) {
	Node* exp = nullptr;
	
	int64 oInt;
	double oDub;
	
	int base = 10;
	int type = parser.ReadInt64(oInt, oDub, base);

	if (type == ZParser::ntInt)
		exp = irg.constIntSigned(oInt, nullptr, base);
	else if (type == ZParser::ntDWord)
		exp = irg.constIntUnsigned(oInt, nullptr, base);
	else if (type == ZParser::ntLong) {
		exp = irg.constIntSigned(oInt);
		exp->SetClass(ass.CLong);
	}
	else if (type == ZParser::ntQWord) {
		exp = irg.constIntUnsigned(oInt);
		exp->SetClass(ass.CQWord);
	}
	else if (type == ZParser::ntSmall)
		exp = irg.constIntUnsigned(oInt, ass.CSmall, base);
	else if (type == ZParser::ntDouble)
		exp = irg.constFloatDouble(oDub);
	else if (type == ZParser::ntFloat)
		exp = irg.constFloatSingle(oDub);
	else if (type == ZParser::ntPtrSize) {
		exp = irg.constIntSigned(oInt);
		exp->SetClass(ass.CPtrSize);
	}
	else
		ASSERT_(0, "Error in parse int");

	return exp;
}

}

