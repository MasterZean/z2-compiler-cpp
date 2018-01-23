#ifndef _z2clib_OverloadResolver_h_
#define _z2clib_OverloadResolver_h_

#include "Assembly.h"

class GatherInfo {
public:
	Overload* Rez = nullptr;
	int Count = 0;
};

class OverloadResolver {
public:
	OverloadResolver(Assembly& a): ass(a) {
	}
	
	Overload* Resolve(Def& def, Vector<Node*>& params, ZClass* spec = nullptr, bool conv = true);
	
	Overload* GatherParIndex(Vector<Overload*>& oo, Vector<Node*>& params, int pi, bool conv);
	void GatherRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot, bool conv);
	void Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot, bool conv);
	void Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot, ObjectType* ot2, bool conv);
	void GatherD(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, bool conv);
	void GatherDRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a, bool conv);
	void GatherDConst(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a, bool conv);
	void GatherDMove(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a, bool conv);
	void GatherS(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, bool conv);
	void GatherR(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a, bool conv);
	Overload* GatherNumeric(Vector<Overload*>& oo, Vector<Node*>& params, int pi, ZClass* cls, bool conv);

	static bool TypesEqualD(Assembly& ass, ObjectType* t1, ObjectType* t2);
	static bool TypesEqualS(Assembly& ass, ObjectType* t1, ObjectType* t2);
	static bool TypesEqualSD(Assembly& ass, ObjectType* t1, ObjectType* t2);
	
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
};

#endif
