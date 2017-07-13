#include "Assembly.h"

bool Node::IsZero(Assembly& ass) {
	if (IsCT == false)
		return false;
	if (ass.IsInteger(Tt) || Tt.Class == ass.CBool || Tt.Class == ass.CChar) {
		if (dInt == 0)
			return true;
	}
	else if (Tt.Class == ass.CFloat || Tt.Class == ass.CDouble) {
		if (dDouble == 0)
			return true;
	}
	else
		ASSERT_(0, "is zero");

	return false;
}

void Node::PromoteToFloatValue(Assembly& ass) {
	if (Tt.Class == ass.CQWord)
		dDouble = (double)(uint32)dInt;
	else if (ass.IsInteger(Tt) || Tt.Class == ass.CBool || Tt.Class == ass.CChar)
		dDouble = (double)dInt;
}

bool ObjectInfo::CanAssign(Assembly& ass, Node* node) {
	return CanAssign(ass, *node,
		node->Tt.Class == ass.CInt && node->NT == NodeType::Const && node->dInt >= 0);
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