#include "OverloadResolver.h"
#include "Node.h"

namespace Z2 {

Overload* OverloadResolver::Resolve(Method& def, Vector<Node*>& params, int limit, ZClass* spec, bool convert) {
	// no result if no overloads exist
	if (def.Overloads.GetCount() == 0)
		return nullptr;
	
	Vector<Overload*> candidates;
	
	Overload *ct = nullptr;
	Overload *mt = nullptr;
	
	for (int ii = 0; ii < def.Overloads.GetCount(); ii++) {
		Overload& over = def.Overloads[ii];
		over.Score = 0;
		
		if (params.GetCount() == over.Params.GetCount()) {
			if (!spec || (spec && over.TParam.GetCount() && over.TParam[0] == spec)) {
				// function that take no parameters are a match by definition
				if (params.GetCount() == 0) {
					if (over.IsConst)
						ct = &over;
					else
						mt = &over;
					
					if (limit == 0)
						return &over;
					else if (limit == 2) {
						if (ct && mt)
							return ct;
					}
					else if (limit == 1) {
						if (ct && mt)
							return mt;
					}
				}
				
				if (limit == 0 || (limit == 2 && over.IsConst) || (limit == 1 && !over.IsConst)) {
					over.Score = 1;
					candidates.Add(&over);
				}
			}
		}
	}
	
	if (ct)
		return ct;
	if (mt)
		return mt;
	
	// no result if no overloads match parameter number
	if (candidates.GetCount() == 0) {
		if (limit == 1)
			return Resolve(def, params, 2, spec, convert);
		else
			return nullptr;
	}
	
	// if specialization if requested, try to look it up for early return
	if (spec && def.Overloads.GetCount() != 0/* && candidates.GetCount() != 0*/) {
		// TODO
		ASSERT(candidates.GetCount() == 1);
		
		Overload& over = *candidates[0];
		bool fail = false;
		
		for (int jj = 0; jj < params.GetCount(); jj++) {
			Node& ap = *params[jj];
			
			if (!ass.CanAssign(over.Params[jj].Class, &ap)) {
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
		
	Overload* result = GatherParIndex(candidates, params, 0);
	
	if (result)
		return result;
	
	if (ambig == 0 && limit == 1)
		return Resolve(def, params, 2, spec, convert);
	
	return nullptr;
}

Overload* OverloadResolver::GatherParIndex(Vector<Overload*>& oo, Vector<Node*>& params, int pi) {
	Node& n = *params[pi];
	
	if (n.Class->MIsNumeric) {
		Overload* o1 = GatherNumeric(oo, params, pi, n.Class);
		if (conv && !o1 && n.C1) {
			o1 = GatherNumeric(oo, params, pi, n.C1);
			if (!o1 && n.C2) {
				o1 = GatherNumeric(oo, params, pi, n.C2);
				return o1;
			}
			else
				return o1;
		}
		else
			return o1;
	}
	
	ASSERT(0);
	return nullptr;
}

Overload* OverloadResolver::GatherNumeric(Vector<Overload*>& oo, Vector<Node*>& params, int pi, ZClass* cls) {
	Node& n = *params[pi];

	if (cls == ass.CInt) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CInt->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		if (n.IsCT && n.IsLiteral && (int)n.IntVal >= 0) {
			gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CPtrSize);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
	}
	else if (cls == ass.CByte) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CByte->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1 && ambig) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CByte);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }

		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CWord, ass.CDWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CQWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CShort, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CFloat, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CWord) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CWord->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CQWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CFloat, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CDWord) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CDWord->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CQWord);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		/*if (n.IsCT && n.IsLiteral && (int)n.IntVal >= 0) {
			gi.Count = 0; Gather(oo, params, pi, gi, n, a, &ass.CPtrSize->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
	}
	else if (cls == ass.CSmall) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyRef, ass.CSmall);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsTemporary) {
			gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyMove, ass.CSmall);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyAuto, ass.CSmall);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyMove, ass.CShort, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyAuto, ass.CShort, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyMove, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyAuto, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyMove, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyAuto, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyMove, ass.CFloat, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherN(oo, params, pi, gi, Variable::tyAuto, ass.CFloat, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CShort) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CShort->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CShort);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CInt);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CLong);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CFloat, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CFloat) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CFloat->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CFloat);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	else if (cls == ass.CDouble) {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		/*if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &ass.CFloat->Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}*/
		
		gi.Count = 0; Gather(oo, params, pi, gi, n, ass.CDouble);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}
	/*else {
		GatherInfo gi;
		gi.Rez = nullptr;
		
		if (n.IsTemporary) {
			gi.Count = 0; GatherDMove(oo, params, pi, gi, a);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		if (n.IsAddressable) {
			gi.Count = 0; GatherRef(oo, params, pi, gi, n, a, &a.Tt);
			if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		}
		
		gi.Count = 0; GatherD(oo, params, pi, gi, n);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
		gi.Count = 0; GatherS(oo, params, pi, gi, n);
		if (gi.Count == 1)	return gi.Rez; else if (gi.Count > 1) { ambig = true; return nullptr; }
	}*/
	
	return nullptr;
}

void OverloadResolver::Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node&n, ZClass* ot) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ZClass* f = over.Params[pi].Class;
		/*if (conv || (!conv && over.Params[pi].FromTemplate == false)) {
			if (f.IsRef) {
				if (f.IsTemporary == false && n.IsAddressable && f.IsConst == a.IsConst) {
					if (f.Tt == ot)
						temp.Add(&over);
				}
			}
			else if (over.Params[pi].PType == Variable::tyAuto || over.Params[pi].PType == Variable::tyVal) {
				if (f.Tt == ot)
					temp.Add(&over);
			}
		}
		else {*/
		if (over.Params[pi].PType == Variable::tyAuto || over.Params[pi].PType == Variable::tyVal)
			if (f == ot)
				temp.Add(&over);
		//}
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


void OverloadResolver::Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ZClass* ot, ZClass* ot2) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ZClass* f = over.Params[pi].Class;
		/*if (conv || (!conv && over.Params[pi].FromTemplate == false))
			if (f.IsRef) {
				if (f.IsTemporary == false && n.IsAddressable && f.IsConst == a.IsConst) {
					if (f.Tt == ot || f.Tt == ot2)
						temp.Add(&over);
				}
			}
			else  */
			if (over.Params[pi].PType == Variable::tyAuto || over.Params[pi].PType == Variable::tyVal)
				if (f == ot || f == ot2)
					temp.Add(&over);
			//}
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

void OverloadResolver::GatherN(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Variable::ParamType pt, ZClass* ot, ZClass* ot2) {
	score++;
	
	Vector<Overload*> temp;
	
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		ZClass* f = over.Params[pi].Class;
		
		if (over.Params[pi].PType == pt)
			if (f == ot || f == ot2)
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

void OverloadResolver::GatherDMove(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ZClass* ot) {
	score++;
	Vector<Overload*> temp;
	for (int i = 0; i < oo.GetCount(); i++) {
		Overload& over = *oo[i];
		Variable& par = over.Params[pi];

		if (par.PType == Variable::tyMove && /*TypesEqualD(ass, &par.I.Tt, &a.Tt)*/par.Class == ot)
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

}
