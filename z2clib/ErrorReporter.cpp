#include "ErrorReporter.h"
#include <z2clib/Assembly.h>

void ErrorReporter::CantAccess(const ZSource& src, const Point& p, Overload& over, const String& cls) {
	String err;
	
	err << "Can't access non public ";
	if (over.IsStatic)
		err << "static ";
	if (over.IsCons)
		err << "constructor";
	else
		err << "function";
	err << ":\n";
	err << "\t\t" << "\f" << cls << "\f";
	if (over.IsCons) {
		if (over.IsCons == 2)
			err << "." << over.Name;
		err << "{" << over.PSig << "}\n";
	}
	else {
		if (over.IsStatic)
			err << "::";
		else
			err << ".";
		err << over.Name;
		err << "(" << over.PSig << ")\n";
	}
	
	Error(src, p, err);
}

void ErrorReporter::CantAccess(const ZSource& src, const Point& p, Constant& over) {
	String err;
	
	err << "Can't access non public ";
	if (over.IsStatic)
		err << "static ";
	if (over.Type == Constant::etVariable)
		err << "variable";
	else
		err << "constant";
	err << ":\n";
	err << "\t\t\f" << over.Name << "\f   \f=>\f   found in class ";
	err << over.Parent->Scan.Namespace << "\f" << over.Parent->Scan.Name << "\f";
	
	Error(src, p, err);
}

void ErrorReporter::Error(const ZSource& src, const Point& p, const String& text) {
	//ASSERT(0);
	throw ZSyntaxError(String().Cat() << src.Package->Path << src.Path << "(" << p.x << ", " << p.y << ")", text);
}

void ErrorReporter::Error(const ZClass& cls, const Point& p, const String& text) {
	Error(*cls.Source, p, text);
}

void ErrorReporter::Dup(const ZSource& src, const Point& p, const Point& p2, const String& text, const String& text2) {
	Error(src, p, "duplicate definition '" + text + "', previous definition was at " +
			text2 + "(" + IntStr(p2.x) + ", " + IntStr(p2.y) + ")");
}

void ErrorReporter::CallError(const ZClass& cls, Point& p, Assembly& ass, ObjectType* ci, Def* def, Vector<Node*>& params, bool cons) {
	String s;
	ObjectInfo i;
	i.Tt = *ci;
	if (cons) {
		s << "Class '\f" << ass.TypeToString(&i) << "\f' does not have a constructor\n\t\t";
	}
	else {
		String z = ass.TypeToString(&i);
		s << "Class '\f" << z << "\f' does not have an overload\n\t\t";
		if (def)
			s << def->Name;
		else
			s << z;
		
	}
	
	if (cons)
		s << "{";
	else
		s << "(";
	for (int k = 0; k < params.GetCount(); k++) {
		s << ass.TypeToString(&params[k]->Tt);
		if (k < params.GetCount() - 1)
			s << ", ";
	}
	if (cons)
		s << "}\n";
	else
		s << ")\n";
	
	if (def && def->Template)
		s << "Class \f" << ass.TypeToString(&i) <<"\f has an incompatible template '" << def->Name << "'";
	else if (def) {
		s << "\texisting overloads\n";
		for (int i = 0; i < def->Overloads.GetCount(); i++) {
			Overload& ol = def->Overloads[i];
			if (ol.IsCons == 1)
				s << '\t\t' << "{" << ol.PSig << "}\n";
			else if (ol.IsCons == 2)
				s << "\t\t" << ol.Name << "{" << ol.PSig << "}\n";
			else
				s << "\t\t" << ol.Name << "(" << ol.PSig << ")\n";
		}
	}
	
	Error(cls, p, s);
}

