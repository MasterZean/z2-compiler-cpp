#include "OverloadResolver.h"
#include "Node.h"

Overload* OverloadResolver::Resolve(Def& def, Vector<Node*>& params, ZClass* spec, bool convert) {
	// no result if no overloads exist
	if (def.Overloads.GetCount() == 0)
		return nullptr;
	
	Vector<Overload*> candidates;
	
	for (int ii = 0; ii < def.Overloads.GetCount(); ii++) {
		Overload& over = def.Overloads[ii];
		over.Score = 0;
		
		if (params.GetCount() == over.Params.GetCount()) {
			if (!spec || (spec && over.TParam.GetCount() && over.TParam[0] == spec)) {
				// function that take no parameters are a match by definition
				if (params.GetCount() == 0)
					return &over;
				
				over.Score = 1;
				candidates.Add(&over);
			}
		}
	}
	
	// no result if no overloads match parameter number
	if (candidates.GetCount() == 0)
		return nullptr;
	
	// if specialization if requested, try to look it up for early return
	if (spec && def.Overloads.GetCount() != 0/* && candidates.GetCount() != 0*/) {
		// TODO
		ASSERT(candidates.GetCount() == 1);
		
		Overload& over = *candidates[0];
		bool fail = false;
		
		for (int jj = 0; jj < params.GetCount(); jj++) {
			Node& ap = *params[jj];
			ObjectInfo& api = ap;
			ObjectInfo& fpi = over.Params[jj].I;
			
			if (!fpi.CanAssign(ass, &ap)) {
				fail = true;
				break;
			}
		}
		
		if (fail)
			return nullptr;
		else
			return &over;
	}
	
	score = 1;
	conv = convert;
	
	return GatherParIndex(candidates, params, 0);
}

Overload* OverloadResolver::GatherParIndex(Vector<Overload*>& oo, Vector<Node*>& params, int pi) {
	Node& n = *params[pi];
	ObjectInfo& a = n;
	
	if (ass.IsNumeric(a.Tt)) {
		Overload* o1 = GatherNumeric(oo, params, pi, a.Tt.Class);
		if (conv && !o1 && a.C1) {
			o1 = GatherNumeric(oo, params, pi, a.C1);
			if (!o1 && a.C2) {
				o1 = GatherNumeric(oo, params, pi, a.C2);
				return o1;
			}
			else
				return o1;
		}
		else
			return o1;
	}
	else if (a.Tt.Class->MIsRawVec) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		gi.Count = 0; GatherD(oo, params, pi, gi, n, a);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		//if (conv) {
			gi.Count = 0; GatherR(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		//}
	}
	else {
		GatherInfo gi;
		gi.Rez = nullptr;

		if (n.IsMove || a.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherDRef(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; GatherD(oo, params, pi, gi, n, a);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		gi.Count = 0; GatherS(oo, params, pi, gi, n, a);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	
	return nullptr;
}

Overload* OverloadResolver::GatherNumeric(Vector<Overload*>& oo, Vector<Node*>& params, int pi, ZClass* cls) {
	Node& n = *params[pi];
	ObjectInfo& a = n;

	if (cls == ass.CInt) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CInt->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		if (n.IsCT && n.IsLiteral && (int)n.IntVal >= 0) {
			gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CPtrSize->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
	}
	else if (cls == ass.CByte) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CByte->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1 && ambig) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CByte->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		//if (conv) {
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CWord->Tt, &ass.CDWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CQWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CShort->Tt, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CFloat->Tt, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		//}
	}
	else if (cls == ass.CWord) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CWord->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CQWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CFloat->Tt, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CDWord) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CDWord->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CQWord->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		if (n.IsCT && n.IsLiteral && (int)n.IntVal >= 0) {
			gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CPtrSize->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
	}
	else if (cls == ass.CSmall) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CSmall->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CSmall->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CShort->Tt, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CFloat->Tt, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CShort) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CShort->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CShort->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CInt->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CLong->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CFloat->Tt, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CFloat) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CFloat->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CFloat->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CDouble->Tt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsMove) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable/* n.LValue || a.IsRef*/) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &a.Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; GatherD(oo, params, pi, gi, n, a);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherS(oo, params, pi, gi, n, a);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	
	return nullptr;
}

void OverloadResolver::GatherRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node&n, ObjectInfo& a, ObjectType* ot) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		//DUMP(over.PSig);
		if (f.IsRef && f.Tt == ot && f.IsMove == false && (f.IsConst == a.IsConst || (f.IsConst && !a.IsConst)))
			temp.Add(&over);
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node&n, ObjectInfo& a, ObjectType* ot) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		if (conv || (!conv && over.Params[pi].FromTemplate == false)) {
			if (f.IsRef) {
				if (f.IsMove == false && (n.IsAddressable  /*sa.IsRef || n.LValue*/) && f.IsConst == a.IsConst) {
					if (f.Tt == ot)
						temp.Add(&over);
				}
			}
			else {
				if (f.Tt == ot)
					temp.Add(&over);
			}
		}
		else {
			if (f.Tt == ot)
				temp.Add(&over);
		}
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}


void OverloadResolver::Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot, ObjectType* ot2) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		if (conv || (!conv && over.Params[pi].FromTemplate == false))
			if (f.IsRef) {
				if (f.IsMove == false && (n.IsAddressable  /*sa.IsRef || n.LValue*/) && f.IsConst == a.IsConst) {
					if (f.Tt == ot || f.Tt == ot2)
						temp.Add(&over);
				}
			}
			else {
				if (f.Tt == ot || f.Tt == ot2)
					temp.Add(&over);
			}
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::GatherD(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		
		if (f.IsRef && !f.IsConst && !a.IsConst && !f.IsMove) {
			if (n.IsAddressable/* a.IsRef || n.LValue*/) {
				if (TypesEqualD(ass, &f.Tt, &a.Tt))
					temp.Add(&over);
			}
		}
		else {
			if (!f.IsMove && (f.IsConst == a.IsConst || (f.IsConst && !a.IsConst)) && TypesEqualD(ass, &f.Tt, &a.Tt))
				temp.Add(&over);
		}
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::GatherDRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		if (f.IsRef && !f.IsMove && !f.IsConst && !a.IsConst && TypesEqualD(ass, &f.Tt, &a.Tt))
			temp.Add(&over);
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::GatherDMove(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		//if (f.IsRef && !f.IsConst && TypesEqualD(ass, &f.Tt, &a.Tt))
		if (f.IsMove && TypesEqualD(ass, &f.Tt, &a.Tt))
			temp.Add(&over);
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::GatherR(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;
		if (f.Tt.Class->MIsRawVec && f.Tt.Class->T == a.Tt.Class->T && f.Tt.Param == -1)
			temp.Add(&over);
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

void OverloadResolver::GatherS(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ObjectInfo& f = over.Params[pi].I;

		if (f.IsRef && !f.IsConst && !f.IsMove) {
			if (n.IsAddressable/* a.IsRef || n.LValue*/) {
				if (TypesEqualS(ass, &f.Tt, &a.Tt))
					temp.Add(&over);
			}
		}
		else {
			if (!f.IsMove && TypesEqualS(ass, &f.Tt, &a.Tt))
				temp.Add(&over);
		}
	}
	
	for (int i = 0; i < temp.GetCount(); i++) {
		Overload& over = *temp[i];
		
		if (pi < params.GetCount() - 1) {
			Overload* ret = GatherParIndex(temp, params, pi + 1);
			
			if (ret && ret != gi.Rez) {
				gi.Rez = ret;
				gi.Count++;
				over.Score = score;
			}
		}
		else {
			gi.Rez = &over;
			gi.Count++;
			over.Score = score;
		}
	}
}

bool OverloadResolver::TypesEqualD(Assembly& ass, ObjectType* t1, ObjectType* t2) {
	if (t1->Class == ass.CPtr && t2->Class == ass.CNull)
		return true;
	
	while (t1) {
		if (t1->Class != t2->Class || t1->Param != t2->Param)
			return false;
		t1 = t1->Next;
		t2 = t2->Next;
	}
	return true;
}

bool OverloadResolver::TypesEqualSD(Assembly& ass, ObjectType* t1, ObjectType* t2) {
	if (t2->Class->Super.Class != nullptr) {
		ZClass* c = t2->Class;
		t2->Class = t2->Class->Super.Class;
		bool b = TypesEqualSD(ass, t1, t2);
		t2->Class = c;
		return b;
	}
	while (t1) {
		if (t1->Class != t2->Class || t1->Param != t2->Param)
			return false;
		t1 = t1->Next;
		t2 = t2->Next;
	}
	return true;
}

bool OverloadResolver::TypesEqualS(Assembly& ass, ObjectType* t1, ObjectType* t2) {
	return TypesEqualSD(ass, t1, t2);
}