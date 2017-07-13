#ifndef _z2clib_enitites_h_
#define _z2clib_enitites_h_

#include "objecttype.h"

class Constant: public Entity, public Moveable<Constant> {
public:
	bool IsEvaluated = false;
	bool InUse = false;
	ZClass* Parent = nullptr;
	CParser::Pos Skip;

	String StringValue(const Assembly& assembly) const;

	Constant() {
		Type = Entity::etConst;
	}
};

class Variable: public Constant, Moveable<Variable>  {
public:
	Node* Body = nullptr;
	Node* List = nullptr;
	bool IsDeclared = false;
	bool FromTemplate = false;
	bool IsCppRef = false;

	Variable() {
		Type = Entity::etVariable;
	}

	Variable& operator=(const Node& o);

	bool IsVoid(Assembly& ass);
};

class Block: Moveable<Block> {
public:
	WithDeepCopy<VectorMap<String, Variable>> Vars;
	int Temps = 0;
};

class Def;

class Overload: public Entity, public Moveable<Overload> {
public:
	enum AutoMode {
		amNotAuto,
		amEqFull,
		amNeqFull,
		amEqAsNeqOposite,
		amNeqAsEqOposite,
		amLessEq,
		amMoreEq,
		amGetter,
		amSetter,
		amCopyCon,
	};
	
	CParser::Pos CPosPar;
	CParser::Pos Skip;
	WithDeepCopy<Vector<Variable>> Params;
	WithDeepCopy<Vector<String>> ParamPreview;
	bool IsDeclared = false;
	int IsCons = 0;
	bool IsGenerated = false;
	bool IsStatic = false;
	bool IsEvaluated = false;
	bool IsConsidered = false;
	ObjectInfo Return;
	WithDeepCopy<Vector<Block>> Blocks;
	String Body;
	bool IsProp = false;
	bool IsGetter = false;
	bool IsIndex = false;
	bool IsNative = false;
	bool IsAlias = false;
	bool IsExtern = false;
	bool IsIntrinsic = false;
	int IsIntrinsicType = -1;
	bool IsDllImport = false;
	bool IsStdCall = false;
	int IsVirtual = false;
	bool IsConst = false;
	Def* Parent = nullptr;
	String BackendName;
	String BackendSuffix;
	String PSig;
	String PBSig;
	String PCSig;
	WithDeepCopy<Index<String>> Initializes;
	int IIndex = 0;
	bool IsDest = false;
	WithDeepCopy<Vector<ZClass*>> TParam;
	int CBOCount = 0;
	bool IsInline = true;
	bool FromTemplate = false;
	bool IsInClassBodyInline = false;
	bool IsOnlyZeroInit = false;
	bool IsVoidReturn = false;
	String PropVarName;
	
	AutoMode IsAuto = amNotAuto;
	String AliasClass;
	String AliasName;
	Point AliasPos;
	
	int Score = 0;
	int Statements = 0;
	int Returns = 0;
	int Loops = 0;
	Variable* ZeroFirstVar = nullptr;
	WithDeepCopy<Index<ZClass*>> CDeps;
};

class Def: public Entity, public Moveable<Def> {
public:
	Array<Overload> Overloads;
	String BackendName;
	ZClass* Class = nullptr;
	Array<CParser::Pos> Pos;
	Vector<int> PC;
	Array<CParser::Pos> BodyPos;
	Overload* HasPGetter = nullptr;
	Overload* HasPSetter = nullptr;
	bool Template = false;
	Vector<String> DTName;
	CParser::Pos CPosPar;

	Def() = default;

	Def(const Def& def) {
		Name = def.Name;
		Location = def.Location;
		Access = def.Access;
		IsStatic = def.IsStatic;
		Class = def.Class;
		BackendName = def.BackendName;
		for (int i = 0; i < def.Overloads.GetCount(); i++) {
			if (!def.Overloads[i].FromTemplate)
				Overloads.Add(def.Overloads[i]);
		}
		Pos <<= def.Pos;
		PC <<= def.PC;
		HasPGetter = def.HasPGetter;
		HasPSetter = def.HasPSetter;
		Template = def.Template;
		//IsConst = def.IsConst;
		BodyPos <<= def.BodyPos;
		DTName <<= def.DTName;
	}

	Overload& Add(const String& name, AccessType insertAccess, bool stat) {
		return Add(name, name, insertAccess, stat);
	}

	Overload& Add(const String& name, const String& bname, AccessType insertAccess, bool stat) {
		Overload& over = Overloads.Add();
		over.IsDeclared = false;
		over.Access = insertAccess;
		over.Parent = this;
		over.Name = name;
		over.BackendName = bname;
		over.IsStatic = stat;
		return over;
	}
};

class ZClassSuper {
public:
	ZClass* Class = nullptr;
	String Name;
	::Point Point;
	bool IsEvaluated = false;
};

class ZClassScanInfo {
public:
	String Name;
	String Namespace;
	String TName;
	
	bool HasDefaultCons = false;
	bool IsEnum = false;
	bool IsTemplate = false;
	bool HasVirtuals = false;
};

class ZClassMeth {
public:
	Overload* CopyCon = nullptr;
	Overload* MoveCon = nullptr;
	Overload* Copy = nullptr;
	Overload* Move = nullptr;
	Overload* Default = nullptr;
	
	Overload* Eq = nullptr;
	Overload* Neq = nullptr;
};

class AST;
class Compiler;

class ZClass: Moveable<ZClass> {
public:
	ZClassScanInfo Scan;
	ZClassMeth Meth;
	String BackendName;
	
	Point Position;
	ZSource* Source = nullptr;
	bool CoreSimple = false;
	bool IsDefined = true;
	ObjectType Tt;
	ObjectType Pt;
	bool IsEvaluated = false;
	
	bool FromTemplate = false;
	ZClass* ParamType = nullptr;
	ZClassSuper Super;
	int Index = -1;
	int RTTIIndex = 0;
	ZClass* T = nullptr;
	ZClass* TBase = nullptr;
	
	Point IPos;
	::ZSource* ISource = nullptr;
	
	int HasEq = 0;
	int HasNeq = 0;
	int HasLess = 0;
	int HasMore = 0;
	int HasLessEq = 0;
	int HasMoreEq = 0;
	Overload* DefWrite = nullptr;
	bool IsPOD = true;

	ArrayMap<String, Constant> Constants;
	ArrayMap<String, Def> Defs;
	ArrayMap<String, Def> Props;
	ArrayMap<String, Def> Cons;
	Def* Dest = nullptr;
	ArrayMap<String, Variable> Vars;

	VectorMap<int, ObjectType*> Raws;
	Vector<ObjectType*> Temps;
	Vector<ZClass*> Children;

	bool MIsNumeric = false;
	bool MIsInteger = false;
	bool MIsRawVec = false;

	String MContName;
	
	int IsWritten = 0;

	ZClass() {
		IPos = Point(-1, -1);
	}

	ZClass(const ZClass& cls) {
		Position = cls.Position;
		BackendName = cls.BackendName;
		Position = cls.Position;
		Source = cls.Source;
		CoreSimple = cls.CoreSimple;
		IsDefined = cls.IsDefined;
		Tt = cls.Tt;
		Pt = cls.Pt;
		IsEvaluated = cls.IsEvaluated;
		Super = cls.Super;
		Index = cls.Index;
		IPos = cls.IPos;
		ISource = cls.ISource;
		T = cls.T;
		TBase = cls.TBase;
		ParamType = cls.ParamType;
		if (cls.Dest)
			Dest = new Def(*cls.Dest);
		else
			Dest = nullptr;

		Constants <<= cls.Constants;
		Defs <<= cls.Defs;
		Props <<= cls.Props;
		Cons <<= cls.Cons;
		Vars <<= cls.Vars;

		Raws <<= cls.Raws;

		Children <<= cls.Children;
		MIsNumeric = cls.MIsNumeric;
		MIsInteger = cls.MIsInteger;
		MIsRawVec = cls.MIsRawVec;
		MContName = cls.MContName;
		IsPOD = cls.IsPOD;
		HasEq = cls.HasEq;
		HasNeq = cls.HasNeq;
		HasLess = cls.HasLess;
		HasMore = cls.HasMore;
		HasLessEq = cls.HasLessEq;
		HasMoreEq = cls.HasMoreEq;
		RTTIIndex = cls.RTTIIndex;
		DefWrite = nullptr;
		Scan = cls.Scan;
		FromTemplate = cls.FromTemplate;
	}

	~ZClass();

	Overload* FindLength();
	Overload* FindIndex();
	
	void TestDup(const Entity& en, ZParser& parser, bool testName, bool testDef = true);
	void TestDup(const String& name, const Point& pos, ZParser& parser, bool testName, bool testDef = true);

	Overload* Get(Overload* over, const String& name, Node*& p1, Node*& p2, Node*& p3, ZClass*& cls, Assembly& ass, AST* ast, Compiler& cmp, const Point& p);
	Overload* Get(Overload* over, const String& name, Node*& p1, ZClass*& cls, Assembly& ass, AST* ast, Compiler& cmp, const Point& p);
	Overload* Get(Overload* over, const String& name, ZClass*& cls, Assembly& ass, AST* ast, Compiler& cmp, const Point& p);

	Def& FindDef(ZParser& parser, const String& name, const Point& pos, bool cons);

	void AddDefCons(ZParser& parser);

	Constant& AddConst(const String& name) {
		Constant& c	= Constants.Add(name);
		c.Parent = this;
		c.Name = name;
		
		return c;
	}

	Variable& AddVar(const String& name) {
		Variable& c	= Vars.Add(name);
		c.Parent = this;
		c.Name = name;
		
		return c;
	}
};

#endif
