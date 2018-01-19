#ifndef _z2clib_OverloadResolver_h_
#define _z2clib_OverloadResolver_h_

#include "Assembly.h"

class OverloadResolver {
public:
	OverloadResolver(Assembly& a): ass(a) {
	}
	
	Overload* Resolve(Def& def, Vector<Node*>& params, ZClass* spec = nullptr);
	
private:
	Assembly& ass;
	int score = 0;
};

#endif
