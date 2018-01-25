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
	
	Overload* GatherParIndex(Vector<Overload*>& oo, Vector<Node*>& params, int pi);
	void GatherRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot);
	void Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot);
	void Gather(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a, ObjectType* ot, ObjectType* ot2);
	void GatherD(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a);
	void GatherDRef(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a);
	void GatherDConst(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a);
	void GatherDMove(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a);
	void GatherS(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, Node& n, ObjectInfo& a);
	void GatherR(Vector<Overload*>& oo, Vector<Node*>& params, int pi, GatherInfo& gi, ObjectInfo& a);
	Overload* GatherNumeric(Vector<Overload*>& oo, Vector<Node*>& params, int pi, ZClass* cls);

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
	bool conv = false;
};

#endif
