#ifndef _z2c_OverloadResolver_h_
#define _z2c_OverloadResolver_h_

#include "Assembly.h"

namespace Z2 {
	
using namespace Upp;

class GatherInfo {
public:
	Overload* Rez = nullptr;
	int Count = 0;
};

class OverloadResolver {
public:
	OverloadResolver(Assembly& a): ass(a) {
	}
	
	Overload* Resolve(Method& def, Vector<Node*>& params, int limit, ZClass* spec = nullptr, bool conv = true);
	
	Overload* GatherParIndex(Vector<Overload*>& oo, Vector<Node*>& params, int pi);
	Overload* GatherNumeric(Vector<Overload*>& oo, Vector<Node*>& params, int pi, ZClass* cls);
	
	void GatherN(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Variable::ParamType pt, ZClass* ot, ZClass* ot2 = nullptr);

	bool IsAmbig() const {
		return ambig;
	}
	
	int Score() const {
		return score;
	}

private:
	Assembly& ass;
	int score = 0;
	bool ambig = false;
	bool conv = false;
};

};

#endif
