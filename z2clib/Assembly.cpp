#include "Assembly.h"
#include "ErrorReporter.h"

String Assembly::TypeToString(ObjectType* tt) {
	String str;
	
	TypeToString(*tt, str);
	
	return str;
}

void Assembly::TypeToString(const ObjectType& type, String& str) {
	if (type.Class->MIsRawVec) {
		int count = 0;
		
		int ii = ClassCounts.Find(CRaw->Scan.Name);
		if (ii != -1)
			count = ClassCounts[ii];
		
		if (count > 1)
			str << type.Class->Scan.Namespace;
		
		str << CRaw->Scan.Name << "<";
		TypeToString(*type.Class->T, str);
		if (type.Param > 0)
			str << ", " << type.Param;
		str << ">";
	}
	else {
		int count = 0;
		
		int ii = ClassCounts.Find(type.Class->Scan.Name);
		if (ii != -1)
			count = ClassCounts[ii];
		
		if (count > 1)
			str << type.Class->Scan.Namespace;
		
		str << type.Class->Scan.Name;
		if (type.Next) {
			str << '<';
			TypeToString(*type.Next, str);
			str << '>';
		}
	}
}

String Constant::StringValue(const Assembly& assembly) const {
	if (I.Tt.Class == assembly.CBool)
		return String().Cat() << (bool)IVal;
	else if (assembly.IsNumeric(I.Tt)) {
		if (I.Tt.Class == assembly.CDWord)
			return String().Cat() << IVal;
		else if (I.Tt.Class == assembly.CByte) {
			byte b = (byte)IVal;
			return String().Cat() << (int)b;
		}
		else if (I.Tt.Class == assembly.CWord) {
			word b = (word)IVal;
			return String().Cat() << (int)b;
		}
		else if (I.Tt.Class == assembly.CFloat)
			return String().Cat() << DVal << 'f';
		else if (I.Tt.Class == assembly.CDouble)
			return String().Cat() << DVal;
		else
			return String().Cat() << IVal;
	}
	else if (I.Tt.Class == assembly.CChar)
		return String().Cat() << IVal;
	else {
		ASSERT(0);
		return "";
	}
}

void ZClass::TestDup(const Entity& en, ZParser& parser, bool testName, bool testDef) {
	TestDup(en.Name, en.Location, parser, testName, testDef);
}

void ZClass::TestDup(const String& name, const Point& pos, ZParser& parser, bool testName, bool testDef) {
	if (testName && name == Scan.Name)
		parser.Dup(pos, Position, Scan.Name + "::" + name);

	int index;

	if (testDef) {
		index = Defs.Find(name);
		if (index != -1)
			parser.Dup(pos, Defs[index].Location, Scan.Name + "::" + name);
	}

	index = Constants.Find(name);

	if (index != -1)
		parser.Dup(pos, Constants[index].Location, Scan.Name + "::" + name);

	index = Vars.Find(name);
	if (index != -1)
		parser.Dup(pos, Vars[index].Location, Scan.Name + "::" + name);

	index = Props.Find(name);
	if (index != -1)
		parser.Dup(pos, Props[index].Location, Scan.Name + "::" + name);
}

Def& ZClass::FindDef(ZParser& parser, const String& name, const Point& pos, bool cons) {
	int i = -1;
	if (cons)
		i = Cons.Find(name);
	else
		i = Defs.Find(name);
	if (i != -1) {
		if (cons)
			return Cons[i];
		else
			return Defs[i];
	}
	else {
		TestDup(name, pos, parser, !cons, false);
		if (!cons)
			return Defs.Add(name);
		else
			return Cons.Add(name);
	}
}

Overload* ZClass::FindLength() {
	for (int i = 0; i < Props.GetCount(); i++) {
		Def& prop = Props[i];
		if (prop.Name == "Length")
			for (int j = 0; j < prop.Overloads.GetCount(); j++) {
				Overload& over = prop.Overloads[j];
				if (over.IsGetter)
					return &over;
			}
	}
	
	return nullptr;
}

Overload* ZClass::FindIndex() {
	for (int i = 0; i < Props.GetCount(); i++) {
		Def& prop = Props[i];
		if (prop.Name == "@index")
			for (int j = 0; j < prop.Overloads.GetCount(); j++) {
				Overload& over = prop.Overloads[j];
				if (over.IsGetter && over.IsIndex)
					return &over;
			}
	}
	
	return nullptr;
}

String Assembly::BTypeToString(ObjectInfo* type, bool qual) {
	String s;
	if (type->IsRef && !type->Tt.Class->MIsRawVec)	{
		s << CPtr->ParamType->Scan.Name << "<" << type->Tt.Class->Scan.Name << ">";
	}
	else if (type->Tt.Class->MIsRawVec) {
		s << CPtr->ParamType->Scan.Name << "<" << type->Tt.Class->T->ParamType->Scan.Name << ">";
		if (type->Tt.Param == -1)
			s << ", " << CPtrSize->ParamType->Scan.Name;
	}
	else {
		s << type->Tt.Class->ParamType->Scan.Name;
		if (qual && type->Tt.Next) {
			s << '<' << type->Tt.Next->Class->ParamType->Scan.Name;
			if (type->Tt.Param)
				s << ", " << IntStr(type->Tt.Param);
			s << '>';
		}
	}
	return s;
}

String Assembly::CTypeToString(ObjectInfo* type, bool qual) {
	String s;
	if (type->IsConst)
		s = "const ";
	else if (type->IsRef && type->IsMove)
		s = "move ";
	else
		s = "mut ";

	if (type->Tt.Class->MIsRawVec) {
		s << "CArray<" << type->Tt.Class->T->Scan.Name;
		if (type->Tt.Param != -1)
			s << ", " << type->Tt.Param;
		s << ">";
	}
	else {
		s << type->Tt.Class->Scan.Namespace;
		s << type->Tt.Class->Scan.Name;
		if (qual && type->Tt.Next) {
			s << '<' << type->Tt.Next->Class->Scan.Name;
			if (type->Tt.Param)
				s << ", " << IntStr(type->Tt.Param);
			s << '>';
		}
	}
	return s;
}

void Assembly::AddSource(ZSource& src) {
	for (int i = 0; i < src.ClassPrototypes.GetCount(); i++) {
		ZClass& cls = src.ClassPrototypes[i];
		String fullName = cls.Scan.Namespace;
		fullName << cls.Scan.Name;
		
		int index = Classes.Find(fullName);
		if (index != -1) {
			ZClass& cls2 = Classes[index];
			if (!cls2.IsDefined) {
				cls2.Children <<= cls.Children;
				cls2.Defs <<= cls.Defs;
				cls2.Vars <<= cls.Vars;
				cls2.Props <<= cls.Props;
				cls2.Constants <<= cls.Constants;
				cls2.Cons <<= cls.Cons;
				cls2.IsDefined = true;
				cls2.Source = cls.Source;
				cls2.Position = cls.Position;
				cls2.Dest = cls.Dest;
				cls2.Meth = cls.Meth;
				cls.Dest = nullptr;
				cls2.Scan = cls.Scan;
				
				SetOwnership(cls2);

				src.Declarations.Add(&cls2);
			}
			else
				ErrorReporter::Dup(src, cls.Position, cls2.Position, cls.Scan.Name, GetFileName(src.Path));
		}
		else {
			ZClass& cls2 = Classes.Add(fullName, cls);
			cls2.Index = Classes.GetCount() - 1;
			cls2.Tt.Class = &cls2;
			cls2.Pt = GetPtr(&cls2.Tt);
			
			SetOwnership(cls2);
			
			src.Declarations.Add(&cls2);
		}
	}
	
	for (int i = 0; i < src.Aliases.GetCount(); i++) {
		ZClassAlias& alias = src.Aliases[i];
		String fullName = alias.Namespace;;
		fullName << alias.Name;
		
		int index = Aliases.Find(fullName);
		if (index != -1) {
			ZClassAlias& second = Aliases[index];
			ErrorReporter::Dup(src, alias.Location, second.Location, fullName);
			return;
		}
		
		Aliases.Add(fullName, alias);
	}
}

void Assembly::AddBuiltInClasses() {
	ASSERT(Classes.GetCount() == 0);

	CCls      = AddCoreType("sys.core.", "Class", "Class", false, false, false);
	CDef      = AddCoreType("sys.core.lang.", "Def", "Def");
	CVoid     = AddCoreType("sys.core.", "Void", "void");
	CNull     = AddCoreType("sys.core.", "Null", "");
	CBool     = AddCoreType("sys.core.lang.", "Bool",     "bool"   ,  true, false);
	CSmall    = AddCoreType("sys.core.lang.", "Small",    "int8"   ,  true, true);
	CByte     = AddCoreType("sys.core.lang.", "Byte" ,    "uint8"  ,  true, true);
	CShort    = AddCoreType("sys.core.lang.", "Short",    "int16"  ,  true, true);
	CWord     = AddCoreType("sys.core.lang.", "Word" ,    "uint16" ,  true, true);
	CInt      = AddCoreType("sys.core.lang.", "Int"  ,    "int32"  ,  true, true);
	CDWord    = AddCoreType("sys.core.lang.", "DWord",    "uint32" ,  true, true);
	CLong     = AddCoreType("sys.core.lang.", "Long" ,    "int64"  ,  true, true);
	CQWord    = AddCoreType("sys.core.lang.", "QWord",    "uint64" ,  true, true);
	CFloat    = AddCoreType("sys.core.lang.", "Float",    "float"  ,  true, false);
	CDouble   = AddCoreType("sys.core.lang.", "Double",   "double" ,  true, false);
	CChar     = AddCoreType("sys.core.lang.", "Char",     "uint32" ,  true, false);
	CChar->ParamType = CDWord;
	CPtrSize  = AddCoreType("sys.core.lang.", "PtrSize",  "size_t",   true, true);
	CPtrSize->ParamType = CDWord;
	CStream   = AddCoreType("sys.core.",      "Stream",   "Stream",   false, false, false);

	CPtr = AddCoreType("sys.core.lang.", "Ptr",      "Ptr");
	CPtr->Scan.IsTemplate = true;
	CString  = AddCoreType("sys.core.lang.", "String", "String", false, false, false);
	CRaw = AddCoreType("sys.core.lang.", "CArray", "", false, false, true);
	CRaw->Scan.IsTemplate = true;
	CRaw->MIsRawVec = true;
	
	CVect     = AddCoreType("sys.core.lang.", "Vector", "Vector", false, false, false);

	CSlice    = AddCoreType("sys.core.lang.", "Slice", "Slice", false, false, false);
	CSlice->Scan.IsTemplate = true;
	
	CIntrinsic = AddCoreType("sys.core.lang.",      "Intrinsic",   "Intrinsic",   false, false, false);

	for (int i = 0; i < Classes.GetCount(); i++)
		Classes[i].Pt = GetPtr(&Classes[i].Tt);
}

ZClass* Assembly::AddCoreType(const String& ns, const String& name, const String& backendName, bool num, bool integer, bool core) {
	ASSERT(ns.EndsWith("."));
	int type = Classes.GetCount();
	ZClass& typeCls = Classes.Add(ns + name);
	typeCls.Scan.Namespace = ns;
	typeCls.MIsNumeric = num;
	typeCls.MIsInteger = integer;
	typeCls.Scan.Name = name;
	typeCls.BackendName = backendName;
	typeCls.MContName = name;
	typeCls.CoreSimple = core;
	typeCls.IsDefined = false;
	typeCls.Index = type;
	typeCls.ParamType = &typeCls;
	typeCls.Tt.Class = &typeCls;

	return &typeCls;
}

bool Assembly::IsSignedInt(ObjectType* type) const {
	return type->Class == CInt || type->Class == CSmall || type->Class == CShort || type->Class == CLong;
}

bool Assembly::IsSignedInt(const ObjectType& type) const {
	return type.Class == CInt || type.Class == CSmall || type.Class == CShort || type.Class == CLong;
}

bool Assembly::IsUnsignedInt(ObjectType* type) const {
	return type->Class == CDWord || type->Class == CByte || type->Class == CWord || type->Class == CQWord;
}

bool Assembly::IsUnsignedInt(const ObjectType& type) const {
	return type.Class == CDWord || type.Class == CByte || type.Class == CWord || type.Class == CQWord;
}

bool Assembly::IsFloat(ObjectType* type) const {
	return type->Class == CFloat || type->Class == CDouble;
}

bool Assembly::IsFloat(const ObjectType& type) const {
	return type.Class == CFloat || type.Class == CDouble;
}

ZClass& Assembly::AddClass(const String& name, const String& nameSpace, ZParser& parser, const Point& pnt) {
	String fullName = nameSpace;
	fullName << name;

	int index = Classes.Find(fullName);

	if (index != -1) {
		if (Classes[index].IsDefined) {
			ZClass& c = Classes[index];
			if (c.Source->Path == parser.Source->Path)
				parser.Dup(pnt, c.Position, name);
			else if (GetFileDirectory(c.Source->Path) == GetFileDirectory(parser.Source->Path))
				parser.Dup(pnt, c.Position, name, GetFileName(c.Source->Path));
			else
				parser.Dup(pnt, c.Position, name, c.Source->Path);

			return c;
		}
		else {
			ZClass& c = Classes[index];
			c.Position = pnt;
			c.IsDefined = true;
			
			return c;
		}
	}
	else {
		ZClass& c = Classes.Add(fullName);
		c.Scan.Name = name;
		c.BackendName = name;
		c.Position = pnt;
		c.Scan.Namespace = nameSpace;
		c.Tt.Class = &Classes[Classes.GetCount() - 1];
		c.Pt = GetPtr(&c.Tt);
		c.Index = Classes.GetCount() - 1;
		c.ParamType = &c;
		c.IsDefined = true;
		
		return c;
	}
}

void Assembly::SetOwnership(ZClass& tclass) {
	for (int i = 0; i < tclass.Constants.GetCount(); i++) {
		Constant& def = tclass.Constants[i];
		def.Parent = &tclass;
	}
	
	for (int i = 0; i < tclass.Vars.GetCount(); i++) {
		Variable& def = tclass.Vars[i];
		def.Parent = &tclass;
	}
	
	for (int i = 0; i < tclass.Defs.GetCount(); i++) {
		::Def& def = tclass.Defs[i];
		def.Class = &tclass;
		for (int j = 0; j < def.Overloads.GetCount(); j++)
			def.Overloads[j].Parent = &def;
	}
	
	for (int i = 0; i < tclass.Cons.GetCount(); i++) {
		::Def& def = tclass.Cons[i];
		def.Class = &tclass;
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			Overload& ol = def.Overloads[j];
			ol.Parent = &def;
			if (ol.IsCons != 2)
				ol.BackendName = def.BackendName;
		}
	}
	
	for (int i = 0; i < tclass.Props.GetCount(); i++) {
		::Def& def = tclass.Props[i];
		def.Class = &tclass;
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			def.Overloads[j].Parent = &def;
			if (&tclass.Props[i].Overloads[j] == tclass.Props[i].HasPGetter)
				def.HasPGetter = &def.Overloads[j];
			if (&tclass.Props[i].Overloads[j] == tclass.Props[i].HasPSetter)
				def.HasPSetter = &def.Overloads[j];
		}
	}
	
	if (tclass.Dest) {
		::Def& def = *tclass.Dest;
		def.Class = &tclass;
		def.Overloads[0].Return.Tt = CVoid->Tt;
		def.BackendName = "~" + tclass.BackendName;
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			def.Overloads[j].Parent = &def;
			def.Overloads[j].BackendName = def.BackendName;
		}
	}
}

ZClass& Assembly::Clone(ZClass& cls, const String& name, const String& bname) {
	int i = Classes.Find(name);
	
	if (i != -1)
		return Classes[i];
	else {
		ZClass& tclass = Classes.Add(name, cls);
		tclass.Scan.Name = name;
		tclass.BackendName = bname;
		tclass.MContName = name;
		tclass.Index = Classes.GetCount() - 1;
		tclass.IsEvaluated = false;

		SetOwnership(tclass);
		
		return tclass;
	}
}

