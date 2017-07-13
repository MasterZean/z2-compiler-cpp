#include "Scanner.h"

void Scanner::Scan(bool cond) {
	while (!parser.IsEof()) {
		if (parser.Id("using"))
			ScanUsing();
		else if (parser.Id("namespace"))
			ScanNamespace();
		else if (parser.Id("alias"))
			ScanAlias();
		else if (parser.Id("class"))
			ScanClass();
		else if (parser.Id("enum"))
			ScanEnum();
		else if (cond && parser.EatIf())
			ScanIf();
		else if (!cond && (parser.IsElse() || parser.IsEndIf()))
			return;
		else {
			Point p = parser.GetPoint();
			parser.Error(p, "syntax error: " + parser.Identify() + " found");
		}
	}
	
	for (int i = 0; i < source.References.GetCount(); i++) {
		if (source.References[i].Find(".") == -1) {
			source.References[i] = nameSpace + source.References[i];
		}
	}
}

void Scanner::ScanIf() {
	String id1 = parser.ExpectId();
	parser.Expect('.');
	String id2 = parser.ExpectId();
	parser.Expect('=');
	parser.Expect('=');
	String id3 = parser.ReadString();
	if (id3 == "WIN32") {
		if (win) {
			Scan(false);
			if (parser.EatElse()) {
				parser.SkipBlock();
				parser.EatEndIf();
			}
			else {
				parser.EatEndIf();
			}
		}
		else {
			parser.SkipBlock();
			if (parser.EatElse()) {
				Scan(false);
				parser.EatEndIf();
			}
			else {
				parser.EatEndIf();
			}
		}
	}
	else {
		
	}
}

void Scanner::ScanIf(ZClass& cls) {
	String id1 = parser.ExpectId();
	parser.Expect('.');
	String id2 = parser.ExpectId();
	parser.Expect('=');
	parser.Expect('=');
	String id3 = parser.ReadString();
	if (id3 == "WIN32") {
		if (win) {
			ClassLoop(cls, false);
			if (parser.EatElse()) {
				parser.SkipBlock();
				parser.EatEndIf();
			}
			else {
				parser.EatEndIf();
			}
		}
		else {
			parser.SkipBlock();
			if (parser.EatElse()) {
				ClassLoop(cls, false);
				parser.EatEndIf();
			}
			else {
				parser.EatEndIf();
			}
		}
	}
	else {
		
	}
}

void Scanner::ScanAlias() {
	Point location = parser.GetPoint();
	
	String name = parser.ExpectId();
	String nspace = nameSpace;
	String fullName = nspace;
	fullName << name;
	
	int index = source.Aliases.Find(fullName);
	if (index != -1) {
		ZClassAlias& alias = source.Aliases[index];
		parser.Dup(location, alias.Location, fullName);
		return;
	}
	
	source.ClassNameList.FindAdd(fullName);
	
	ZClassAlias& alias = source.Aliases.Add(fullName);
	alias.Name = name;
	alias.Namespace = nspace;
	alias.Location = location;
	alias.Source = &source;
	
	parser.Expect('=');
	alias.Context = parser.GetPos();
	ScanType();
	
	parser.Expect(';');
}

void Scanner::ScanUsing() {
	Point p = parser.GetPoint();
	String path = parser.ExpectId();
	String last = "";
	while (parser.Char('.')) {
		if (last.GetLength() != 0)
			path << '.' << last;
		last = parser.ExpectId();
	}
	if (last.GetLength()) {
		path << '.';
		path << last;
	}
	parser.Expect(';');
	
	source.References.Add(path);
	source.ReferencePos.Add(p);
}

void Scanner::ScanNamespace() {
	String path = parser.ExpectId();
	String total = path;
	total << ".";
	
	while (parser.Char('.')) {
		path = parser.ExpectId();
		total << path << ".";
	}

	nameSpace = total;
	
	parser.Expect(';');
}

void Scanner::ScanClass() {
	Point pos = parser.GetPoint();
	String name = parser.ExpectId();
	String tname;
	
	bool tplate = false;
	if (parser.Char('<')) {
		tname = parser.ExpectId();
		parser.Expect(':');
		parser.ExpectId("Class");
		parser.Expect('>');
		tplate = true;
	}

	ZClass& cls = source.AddClass(name, nameSpace, parser, pos);
	cls.Scan.IsTemplate = tplate;
	cls.Scan.TName = tname;
	cls.MContName = cls.Scan.Name;
	pos = parser.GetPoint();
	if (parser.Char(':')) {
		cls.Super.Point = pos;
		cls.Super.Name = parser.ExpectId();
		cls.Super.IsEvaluated = false;
	}
	else
		cls.Super.IsEvaluated = true;
	cls.Source = &source;
	
	String fullName = cls.Scan.Namespace;
	fullName << cls.Scan.Name;
	
	source.References.Add(fullName);
	source.ReferencePos.Add(cls.Position);

	source.ClassNameList.FindAdd(fullName);
	
	insertAccess = Entity::atPublic;
	
	parser.Expect('{');
	ClassLoop(cls, true);

	while (true) {
		if (parser.Id("private")) {
			insertAccess = Entity::atPrivate;
			parser.Expect('{');
			ClassLoop(cls, true);
		}
		else if (parser.Id("protected")) {
			insertAccess = Entity::atProtected;
			parser.Expect('{');
			ClassLoop(cls, true);
		}
		else
			break;
	}

	for (int i = 0; i < cls.Props.GetCount(); i++) {
		Def& def = cls.Props[i];
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			Overload& ol = def.Overloads[j];
			if (ol.IsGetter == false && ol.Params.GetCount())
				for (int k = 0; k < ol.Params.GetCount(); k++) {
					for (int m = 0; m < cls.Vars.GetCount(); m++)
						if (cls.Vars[m].Name == ol.Params[k].Name)
							parser.Dup(ol.Params[k].Location, cls.Vars[m].Location, cls.Vars[m].Name);
				}
		}
	}
}

void Scanner::ScanEnum() {
	Point pos = parser.GetPoint();
	String name = parser.ExpectId();
	ZClass& cls = source.AddClass(name, nameSpace, parser, pos);
	cls.MContName = cls.Scan.Name;
	cls.BackendName = cls.Scan.Name;//"int32";
	cls.CoreSimple = true;
	pos = parser.GetPoint();
	cls.Source = &source;
	String fullName = cls.Scan.Namespace;
	fullName << cls.Scan.Name;
	source.References.Add(fullName);
	source.ReferencePos.Add(cls.Position);
	
	source.ClassNameList.FindAdd(fullName);
	
	insertAccess = Entity::atPublic;
	cls.Scan.IsEnum = true;
	
	parser.Expect('{');
	while (!parser.IsChar('}')) {
		Point p = parser.GetPoint();
		int n = 0;
		while (parser.IsId()) {
			Point pnt = parser.GetPoint();
			String name = parser.ExpectId();
			cls.TestDup(name, pnt, parser, false);

			Constant& cst = cls.AddConst(name);
			cst.Location = pnt;
			cst.Skip = parser.GetPos();
			cst.Access = insertAccess;
			//cst.I.Tt = *cls.Tt;
			cst.IsEvaluated = true;
			cst.IVal = n++;

			if (parser.Char(',')) {
			}
			else if (parser.IsChar('}')) {
			}
			else
				parser.Error(pnt, "syntax error: " + parser.Identify() + " found");
		}
	}
	parser.Expect('}');

	while (true) {
		if (parser.Id("private")) {
			insertAccess = Entity::atPrivate;
			EnumLoop(cls);
		}
		else if (parser.Id("protected")) {
			insertAccess = Entity::atProtected;
			EnumLoop(cls);
		}
		else if (parser.Id("public")) {
			insertAccess = Entity::atPublic;
			EnumLoop(cls);
		}
		else
			break;
	}

	//if (!hasDefaultCons && !cls.CoreSimple)
	//	cls.AddDefCons(parser);
}

void Scanner::ClassLoop(ZClass& cls, bool cond) {
	while (!parser.IsChar('}')) {
		Point p = parser.GetPoint();
		if (parser.Id("def"))
			ScanDef(cls, false);
		else if (parser.Id("func"))
			ScanDef(cls, false, false, 0, true);
		else if (parser.Id("this"))
			ScanDef(cls, true);
		else if (parser.Id("const"))
			ScanConst(cls);
		else if (parser.Id("val"))
			ScanVar(cls);
		else if (parser.Id("property"))
			ScanProperty(cls);
		else if (parser.Id("static")) {
			if (parser.IsId("const"))
				parser.Error(p, "constants are already static");
			else if (parser.Id("def"))
				ScanDef(cls, false, true);
			else if (parser.Id("func"))
				ScanDef(cls, false, true, 0, true);
			else if (parser.IsId("this"))
				//ScanDef(cls, true, true);
				parser.Error(p, "current version of the compiler does not support static constructors");
			else if (parser.Id("val"))
				ScanVar(cls, true);
			else if (parser.Id("property"))
				ScanProperty(cls, true);
			else if (parser.Id("virtual") || parser.Id("override"))
				parser.Error(p, "virtual methods can't be static");
		}
		else if (parser.Id("virtual")) {
			parser.ExpectId("def");
			cls.Scan.HasVirtuals = true;
			ScanDef(cls, false, false, 1);
		}
		else if (parser.Id("override")) {
			if (cls.Super.Name.GetLength() == 0)
				parser.Error(p, "class '\f" + cls.Scan.Name + "\f' does not use inheritence, there is nothing to override");
			parser.ExpectId("def");
			cls.Scan.HasVirtuals = true;
			ScanDef(cls, false, false, 2);
		}
		else if (cond && parser.EatIf())
			ScanIf(cls);
		else if (!cond && (parser.IsElse() || parser.IsEndIf()))
			return;
		else if (parser.Char('#')) {
			if (parser.Id("region"))
				parser.ExpectId();
			else if (!parser.Id("endregion")) {
				Point p2 = parser.GetPoint();
				parser.Error(p2, "syntax error: # followed by invalid directive");
			}
		}
		else if (parser.Char('~')) {
			parser.ExpectId("this");
			parser.Expect('(');

			if (cls.Dest)
				parser.Dup(p, cls.Dest->Location, cls.Scan.Name + "::~");
			cls.Dest = new Def();
			cls.Dest->Name = "~this";
			cls.Dest->Class = &cls;
			Overload& over = cls.Dest->Add("~this", "~" + cls.BackendName, insertAccess, false);
			over.Skip = parser.GetPos();
			over.CPosPar = parser.GetPos();
			over.Location = parser.GetPoint();
			over.IsDeclared = true;
			//over.Return.Tt = *ass.Void;
			over.IsDest = true;

			parser.Expect(')');
			parser.Expect('{');
			over.Skip = parser.GetPos();
			ScanBlock();
		}
		else
			parser.Error(p, "syntax error: constant, variable, method, constructor or property definiton expected, " + parser.Identify() + " found");
	}

	parser.Expect('}');
}

void Scanner::EnumLoop(ZClass& cls) {
	parser.Expect('{');

	while (!parser.IsChar('}')) {
		Point p = parser.GetPoint();
		if (parser.Id("def"))
			ScanDef(cls, false);
		else if (parser.Id("func"))
			ScanDef(cls, false, false, 0, true);
		else if (parser.Id("this"))
			ScanDef(cls, true);
		else if (parser.Id("property"))
			ScanProperty(cls);
		else if (parser.Id("static")) {
			if (parser.IsId("const"))
				parser.Error(p, "constants are already static");
			else if (parser.Id("def"))
				ScanDef(cls, false, true);
			else if (parser.IsId("this"))
				parser.Error(p, "current version of the compiler does not support static constructors");
			else if (parser.Id("val"))
				ScanVar(cls, true);
			else if (parser.IsId("property"))
				ScanProperty(cls, true);
		}
		else if (parser.Char('#')) {
			if (parser.Id("region"))
				parser.ExpectId();
			else if (!parser.Id("endregion"))
				parser.Error(p, "syntax error: # followed by invalid directive");
		}
		else
			parser.Error(p, "syntax error: constant, variable, method, constructor or property definiton expected, " + parser.Identify() + " found");
	}

	parser.Expect('}');
}

void Scanner::ScanConst(ZClass& cls) {
	Point pnt = parser.GetPoint();
	String name = parser.ExpectId();
	cls.TestDup(name, pnt, parser, false);

	Constant& cst = cls.AddConst(name);
	cst.Location = pnt;
	cst.Skip = parser.GetPos();
	cst.Access = insertAccess;

	if (parser.Char(':'))
		ScanType();

	if (parser.Char('='))
		while (!parser.IsChar(';'))
			ScanToken();

	parser.Expect(';');
}

void Scanner::ScanVar(ZClass& cls, bool stat) {
	Point pnt = parser.GetPoint();
	String name = parser.ExpectId();
	cls.TestDup(name, pnt, parser, true);

	Variable& var = cls.AddVar(name);
	var.Location = pnt;
	var.Skip = parser.GetPos();
	var.Access = insertAccess;
	var.IsStatic = stat;

	if (parser.Char(':'))
		ScanType();

	if (parser.Char('='))
		while (!parser.IsChar(';'))
			ScanToken();

	parser.Expect(';');
}

void Scanner::ScanProperty(ZClass& cls, bool stat) {
	Point pnt = parser.GetPoint();
	String name, bname;
	if (parser.Char('@')) {
		String s = parser.ExpectId();
		if (s != "index")
			parser.Error(pnt, "invalid operator for overloading");
		name = "@" + s;
		bname = "_" + s;
	}
	else {
		name = parser.ExpectId();
		bname = name;
	}
	cls.TestDup(name, pnt, parser, true);

	Def& prop = cls.Props.Add(name);
	prop.Name = name;
	prop.BackendName = bname;
	prop.Class = &cls;
	if (prop.Pos.GetCount() == 0)
		prop.Pos.Add(parser.GetPos());
	prop.IsStatic = stat;
	prop.Location = pnt;

	if (parser.Char('=')) {
		parser.ExpectId();
		parser.Expect(';');
		return;
	}
	
	parser.Expect(':');
	if (parser.Id("const"))
		parser.ExpectId("ref");
	ScanType();

	int get = 0, set = 0;
	while (true) {
		if (parser.Id("get")) {
			Overload& over = prop.Add(name, insertAccess, stat);
			over.Location = pnt;
			over.Skip = parser.GetPos();
			over.CPosPar = parser.GetPos();
			over.IsProp = true;
			over.IsGetter = true;
			over.BackendName = bname;
			prop.HasPGetter = &over;

			if (parser.Char('[')) {
				Variable& p = over.Params.Add();
				p.Name = parser.ExpectId();
				parser.Expect(':');
				p.Skip = parser.GetPos();
				ScanType();
				parser.Expect(']');
				over.IsIndex = true;
			}

			while (true) {
				if (parser.Id("Intrinsic")) {
					parser.Expect('.');
					parser.ReadId();
					over.IsNative = true;
				}
				else if (parser.Id("const"))
					over.IsConst = true;
				else if (parser.IsChar('{') || parser.IsChar(';'))
					break;
				else
					parser.Error(pnt, "syntax error: " + parser.Identify() + " found");
			}

			if (over.IsNative)
				parser.Expect(';');
			else {
				parser.Expect('{');
				over.Skip = parser.GetPos();
				ScanBlock();
			}
			get++;
		}
		else if (parser.Id("set")) {
			parser.Expect('(');
			Point p2 = parser.GetPoint();
			String param = parser.ExpectId();
			parser.Expect(')');

			Overload& over = prop.Add(name, insertAccess, stat);
			over.BackendName = bname;
			over.Location = pnt;
			over.Skip = parser.GetPos();
			over.CPosPar = parser.GetPos();
			over.IsProp = true;
			over.IsGetter = false;
			prop.HasPSetter = &over;

			if (parser.Char('[')) {
				Variable& p = over.Params.Add();
				p.Name = parser.ExpectId();
				parser.Expect(':');
				p.Skip = parser.GetPos();
				ScanType();
				parser.Expect(']');
				over.IsIndex = true;
			}

			Variable& p = over.Params.Add();
			p.Name = param;
			p.Location = p2;

			while (true) {
				if (parser.IsId("native")) {
					parser.ReadId();
					over.IsNative = true;
				}
				else if (parser.Id("const"))
					parser.Error(pnt, "setters can't be const");
				else if (parser.IsChar('{'))
					break;
				else
					parser.Error(pnt, "syntax error: " + parser.Identify() + " found");
			}

			parser.Expect('{');
			over.Skip = parser.GetPos();
			ScanBlock();
			set++;
		}
		else if (parser.Char('{')) {
			Overload& over = prop.Add(name, insertAccess, stat);
			over.Location = pnt;
			over.Skip = parser.GetPos();
			over.CPosPar = parser.GetPos();
			over.IsProp = true;
			over.IsGetter = true;
			over.BackendName = bname;
			prop.HasPGetter = &over;

			over.Skip = parser.GetPos();
			ScanBlock();
			get++;
		}
		else {
			if (get == 0 && set == 0)
				parser.Error(pnt, "property '" + name + "' needs to have at least a setter or a getter");
			if (get > 1)
				parser.Error(pnt, "property '" + name + "' can't have more than one getter");
			if (set > 1)
				parser.Error(pnt, "property '" + name + "' can't have more than one setter");
			break;
		}
	}
}

Def& Scanner::ScanDef(ZClass& cls, bool cons, bool stat, int virt, bool cst) {
	Point p = parser.GetPoint();
	String name;
	String bname;
	int cval = 0;
	if (!cons) {
		if (parser.Char('@')) {
			String s = parser.ExpectId();
			if (s == "size")
				parser.Error(p, "'@size' can't be overlaoded");
			name = "@" + s;
			bname = "_" + s;
		}
		else {
			name = parser.ExpectId();
			bname = name;
		}
		if (name == "this")
			parser.Error(p, "identifier expected, 'this' found. Are you trying to define a constructor?");
	}
	else {
		if (parser.IsId()) {
			name = parser.ReadId();
			if (name == cls.Scan.Name)
				parser.Error(p, "named constructor must not have the same name as the parent class");
			cval = 2;
			bname = /*"_" +*/ name;
		}
		else if (parser.Char('@')) {
			name = parser.ReadId();
			cval = 2;
			bname = "_" + name;
			name = "@" + name;
		}
		else {
			name = "this";//cls.Name;
			if (cls.CoreSimple)
				bname << "_" << name;
			else
				bname = name;
			cval = 1;
		}
	}
	
	bool tplt = false;
	Vector<String> tname;
	if (parser.Char('<')) {
		while (true) {
			tname.Add(parser.ExpectId());
			tplt = true;
			parser.Expect(':');
			parser.ExpectId("Class");
			if (parser.Char('>'))
				break;
			parser.Expect(',');
		}
	}
	
	parser.Expect('(');

	Def& def = cls.FindDef(parser, name, p, cons);
	def.Name = name;
	def.BackendName = bname;
	def.IsStatic = stat;
	def.Class = &cls;
	def.Location = p;
	def.Template = tplt;
	def.DTName = clone(tname);
	def.Access = insertAccess;
	CParser::Pos backPos = parser.GetPos();
	if (def.Pos.GetCount() == 0 || def.Template)
		def.Pos.Add(backPos);

	if (cval == 1 && parser.IsChar(')'))
		cls.Scan.HasDefaultCons = true;

	int pc = 0;
	Vector<String> params;
	
	while (!parser.IsChar(')')) {
		if (parser.IsId("val"))
			parser.ReadId();
		else if (cst == false && parser.IsId("const"))
			parser.ReadId();
		else if (parser.IsId("ref"))
			parser.ReadId();
		else if (parser.IsId("move"))
			parser.ReadId();
		params << parser.ExpectId();
		parser.Expect(':');
		ScanType();
		if (parser.Char(',')) {
			if (parser.IsChar(')'))
				parser.Error(parser.GetPoint(), "identifier expected, " + parser.Identify() + " found");
		}
		pc++;
	}
	if (def.Template)
		def.PC.Add(pc);
	
	parser.Expect(')');

	bool hasRet = false;
	if (parser.Char(':')) {
		hasRet = true;
		if (parser.IsId("ref"))
			parser.ReadId();
		ScanType();
	}

	Overload* ol = nullptr;

	if (!tplt) {
		Overload& over = def.Add(name, bname, insertAccess, stat);
		over.Location = p;
		over.Skip = backPos;
		over.CPosPar = backPos;
		over.IsCons = cval;
		over.IsVirtual = virt;
		over.IsConst = cst;
		over.ParamPreview = pick(params);
		over.IsVoidReturn = !hasRet;
		over.IsStatic = def.IsStatic;
		if (cons)
			over.BackendSuffix = "_";

		ol = &over;
	}
	else {
		def.CPosPar = backPos;
	}

	if (!tplt && parser.Char(';')) {
		while (parser.IsId()) {
			Point ap = parser.GetPoint();
			String trait = parser.ReadId();
			if (trait == "native")
				ol->IsNative = true;
			else if (trait == "Intrinsic") {
				ol->AliasClass = "Intrinsic";
				parser.Expect('.');
				ol->AliasName = parser.ExpectId();
				ol->AliasPos = ap;
				ol->IsAlias = true;
			}
			else if (trait == "intrinsic") {
				if (nameSpace == "sys.core.lang." && cls.Scan.Name == "Intrinsic")
					ol->IsIntrinsic = true;
				else
					parser.Error(ap, "only '\fsys.core.lang.Intrinisc\f' can have intrinisc methods.");
			}
			else if (trait == "extern")
				ol->IsExtern = true;
			else if (trait == "const")
				parser.Error(parser.GetPoint(), "Class methods can't be 'const'. Use the 'func' keyword instead.");
			else if (trait == "dllimport")
				ol->IsDllImport = true;
			else if (trait == "stdcall")
				ol->IsStdCall = true;
		}

		if (ol->IsDllImport || ol->IsNative || ol->IsAlias || ol->IsIntrinsic || ol->IsExtern) {
			ol->Skip = parser.GetPos();
			parser.Expect(';');
			return def;
		}
	}
	else if (tplt && parser.Char(';')) {
		while (parser.IsId()) {
			String trait = parser.ReadId();
			if (trait == "const") {
				parser.Error(parser.GetPoint(), "Class methods can't be 'const'. Use the 'func' keyword instead.");
			}
			//def.IsConst = true;
		}
	}

	parser.Expect('{');
	if (def.BodyPos.GetCount() == 0 || def.Template)
		def.BodyPos.Add(parser.GetPos());
	if (ol)
		ol->Skip = parser.GetPos();

	ScanBlock();

	return def;
}

void Scanner::ScanBlock() {
	while (!parser.IsChar('}')) {
		if (parser.Char('{'))
		    ScanBlock();
		else
			ScanToken();
	}
	parser.Expect('}');
}

void Scanner::ScanType() {
	if (parser.Char('[')) {
		parser.ExpectId("c");
		if (parser.IsNumber())
			parser.ReadNumber();
		parser.Expect(']');
		parser.ExpectId();
	}
	else if (/*parser.Char('&') || parser.Char('#') || */parser.Id("val") || parser.Id("ref") || parser.Char('*')) {
		if (parser.Char('[')) {
			parser.ExpectId("c");
			if (parser.IsNumber())
				parser.ReadNumber();
			parser.Expect(']');
			parser.ExpectId();
		}
		else {
			parser.Char('#');
			parser.ExpectId();
			if (parser.Char('<')) {
				parser.ExpectId();
				parser.Expect('>');
			}
		}
	}
	else {
		parser.ExpectId();
		if (parser.Char('<')) {
			String ss = parser.ExpectId();
			if (ss == "const")
				parser.ExpectId();
			if (parser.Char(','))
				ScanToken();
			parser.Expect('>');
		}
	}
}

char tab1[24] = { '+', '-', '*', '/', '=', ';', '(', ')', '.', '<', '>', '&', ',', '%', '|', '^', ':', '!', '[', ']', '@', '~', '?', '#' };
char tab2[9]  = { '<', '>', '=', '!', '<', '>', ':', '+', '-' };
char tab3[9]  = { '<', '>', '=', '=', '=', '=', ':', '+', '-' };

void Scanner::ScanToken() {
	if (parser.IsInt()) {
		int64 oInt;
		double oDub;
		parser.ReadInt64(oInt, oDub);
	}
	else if (parser.IsString())
		parser.ReadString();
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
		if (parser.Char('{') || parser.Char('}'))
			return;

		Point p = parser.GetPoint();
		parser.Error(p, "syntax error: " + parser.Identify() + " found");
	}
}