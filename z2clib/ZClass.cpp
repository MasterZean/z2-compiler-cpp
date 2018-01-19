#include "Assembly.h"

String dummy = ") {}";

void ZClass::AddDefCons(ZParser& parser) {
	String s = "this";

	Def& def = FindDef(parser, s, Position, true);
	def.Name = s;
	def.Class = this;
	Overload& ol = def.Overloads.Insert(0);
	ol.Name = s;
	ol.BackendName = Scan.Name;
	ol.IsCons = 1;
	ol.IsGenerated = true;
	ZParser pr(dummy);

	ol.CPosPar = pr.GetPos();
	pr.Expect(')');
	pr.Expect('{');
	ol.Skip = pr.GetPos();
	ol.Parent = &def;
	ol.Location = def.Location;
	
	Scan.HasDefaultCons = true;
	Meth.Default = &ol;
}

ZClass::~ZClass() {
	for (int i = 0; i < Raws.GetCount(); i++)
		delete Raws.GetValues()[i];
	
	for (int j = 0; j < Temps.GetCount(); j++)
		delete Temps[j];
	
	delete Dest;
	Dest = nullptr;
}