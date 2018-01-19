#include "OverloadResolver.h"
#include "Node.h"

Overload* OverloadResolver::Resolve(Def& def, Vector<Node*>& params, ZClass* spec) {
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
	if (spec && def.Overloads.GetCount() != 0 && candidates.GetCount() != 0) {
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
	
	return nullptr;
}
