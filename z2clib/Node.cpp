#include "Node.h"
#include "Assembly.h"

bool ObjectInfo::CanAssign(Assembly& ass, Node* node) {
	return CanAssign(ass, *node,
		node->Tt.Class == ass.CInt && node->NT == NodeType::Const && node->IntVal >= 0);
}

Variable& Variable::operator=(const Node& o) {
	I.Tt = o.Tt;
	I.IsConst = o.IsConst;
	I.IsRef = o.IsRef;
	return *this;
}

bool Variable::IsVoid(Assembly& ass) {
	return Body && Body->NT == NodeType::Const && Body->IsLiteral && Body->Tt.Class == ass.CVoid;
}

bool Node::IsZero(Assembly& ass) {
	if (IsCT == false)
		return false;
	if (ass.IsInteger(Tt) || Tt.Class == ass.CBool || Tt.Class == ass.CChar) {
		if (IntVal == 0)
			return true;
	}
	else if (Tt.Class == ass.CFloat || Tt.Class == ass.CDouble) {
		if (DblVal == 0)
			return true;
	}
	else
		ASSERT_(0, "is zero");

	return false;
}

void Node::PromoteToFloatValue(Assembly& ass) {
	if (Tt.Class == ass.CQWord)
		DblVal = (double)(uint32)IntVal;
	else if (ass.IsInteger(Tt) || Tt.Class == ass.CBool || Tt.Class == ass.CChar)
		DblVal = (double)IntVal;
}