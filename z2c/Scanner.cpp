#include "Scanner.h"

namespace Z2 {
	
void Scanner::Scan(ZSource& src) {
	ZParser parser;
	parser.Set(src.Data);
	parser.Path = src.Path;
	
	parser.WS();
	
	while (!parser.IsEof()) {
		if (parser.Id("namespace")) {
			parser.WSCurrentLine();
			
			String total = parser.ExpectZId();
			total << ".";

			while (parser.Char('.')) {
				parser.WSCurrentLine();
				total << parser.ExpectZId() << ".";
				parser.WSCurrentLine();
			}
			
			parser.WS();
		
			src.Namespace = total;
		}
		else if (parser.Id("def"))
			ScanDef(src.Module, parser, false);
		else if (parser.Id("func"))
			ScanDef(src.Module, parser, true);
		else if (parser.Id("val")) {
			parser.WSCurrentLine();
			
			ZParser::Pos p = parser.GetPos();
			
			if (parser.IsZId()) {
				String name = parser.ReadZId();
				
				Variable& v = src.Module.AddVariable(name);
				v.Name = name;
				v.EntryPos = p;
			}
			else
				ScanToken(parser);
		}
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
		else if (parser.Char('{')) {
			parser.WS();
			
			int o = 1;
			int c = 0;
			
			while (!parser.IsEof()) {
				if (parser.Char('{')) {
					parser.WS();
					o++;
				}
				else if (parser.Char('}')) {
					parser.WS();
					c++;
					
					if (o == c)
						break;
				}
				else
					ScanToken(parser);
			}
		}
		else
			ScanToken(parser);
	}
}

void Scanner::ScanClass(ZClass& conCls, ZParser& parser) {
	int o = 1;
	int c = 0;
	
	while (!parser.IsEof()) {
		if (parser.Id("def"))
			ScanDef(conCls, parser, false);
		else if (parser.Id("func"))
			ScanDef(conCls, parser, true);
		else if (parser.Id("val")) {
			parser.WSCurrentLine();
			
			ZParser::Pos p = parser.GetPos();
			
			if (parser.IsZId()) {
				String name = parser.ReadZId();
				
				Variable& v = conCls.AddVariable(name);
				v.Name = name;
				v.EntryPos = p;
			}
			else
				ScanToken(parser);
		}
		else if (parser.Char('{')) {
			parser.WS();
			o++;
		}
		else if (parser.Char('}')) {
			parser.WS();
			c++;
			
			if (o == c)
				return;
		}
		else
			ScanToken(parser);
	}
}

void Scanner::ScanDef(ZClass& conCls, ZParser& parser, bool ct) {
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
void Scanner::ScanToken(ZParser& parser) {
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

}