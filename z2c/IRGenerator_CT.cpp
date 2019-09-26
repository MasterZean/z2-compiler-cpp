#include "Assembly.h"

#include "IRGenerator.h"
#include "Assembly.h"
#include "tables.h"

namespace Z2 {

Node* IRGenerator::opAritCT(Node* left, Node* right, OpNode::Type op, ZClass* cls, ZClass* e, int64& dInt, double& dDouble) {
	if (op == OpNode::opAdd) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal + (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal + (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal + (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal + (uint64)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal + (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal + (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal + (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal + (int64)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal + (uint64)right->IntVal;
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal + right->DblVal;
			return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal + right->DblVal;
			return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal + (int32)right->IntVal);
			return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opSub) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal - (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal - (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal - (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal - (uint64)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal - (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal - (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal - (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal - (int64)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal - (uint64)right->IntVal;
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal - right->DblVal;
			return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal - right->DblVal;
			return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal - (int32)right->IntVal);
			return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opMul) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal * (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal * (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal * (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal * (uint64)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal * (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal * (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal * (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal * (int64)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal * (uint64)right->IntVal;
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal * right->DblVal;
			return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal * right->DblVal;
			return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal * (int32)right->IntVal);
			return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opDiv) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal / (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal / (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal / (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal / (uint64)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal / (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal / (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal / (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal / (int64)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal / (uint64)right->IntVal;
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal / right->DblVal;
			return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal / right->DblVal;
			return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal / (int32)right->IntVal);
			return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opMod) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal % (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal % (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal % (uint32)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal % (uint64)right->IntVal);
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal % (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal % (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal % (int32)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal % (int64)right->IntVal);
			return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal % (uint64)right->IntVal;
			return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = fmod(left->DblVal, right->DblVal);
			return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = fmod(left->DblVal, right->DblVal);
			return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal % (int32)right->IntVal);
			return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else
		ASSERT(0);
	
	return nullptr;
}

Node* IRGenerator::opRelCT(Node* left, Node* right, OpNode::Type op, ZClass* e) {
	bool b;
	
	if (e == ass.CByte || e == ass.CWord || e == ass.CDWord) {
		if (op == OpNode::opLess)
			b = (uint64)left->IntVal < (uint64)right->IntVal;
		else if (op == OpNode::opLessEq)
			b = (uint64)left->IntVal <= (uint64)right->IntVal;
		else if (op == OpNode::opMore)
			b = (uint64)left->IntVal > (uint64)right->IntVal;
		else if (op == OpNode::opMoreEq)
			b = (uint64)left->IntVal >= (uint64)right->IntVal;
		else if (op == OpNode::opEq)
			b = (uint64)left->IntVal == (uint64)right->IntVal;
		else if (op == OpNode::opNeq)
			b = (uint64)left->IntVal != (uint64)right->IntVal;
		else
			ASSERT(0);
		
		return constBool(b);
	}
	else if (e == ass.CSmall || e == ass.CShort || e == ass.CInt) {
		if (op == OpNode::opLess)
			b = left->IntVal < right->IntVal;
		else if (op == OpNode::opLessEq)
			b = left->IntVal <= right->IntVal;
		else if (op == OpNode::opMore)
			b = left->IntVal > right->IntVal;
		else if (op == OpNode::opMoreEq)
			b = left->IntVal >= right->IntVal;
		else if (op == OpNode::opEq)
			b = left->IntVal == right->IntVal;
		else if (op == OpNode::opNeq)
			b = left->IntVal != right->IntVal;
		else
			ASSERT(0);
		
		return constBool(b);
	}
	else if (e == ass.CFloat) {
		left->PromoteToFloatValue(ass);
		right->PromoteToFloatValue(ass);
		if (op == OpNode::opLess)
			b = left->DblVal < right->DblVal;
		else if (op == OpNode::opLessEq)
			b = left->DblVal <= right->DblVal;
		else if (op == OpNode::opMore)
			b = left->DblVal > right->DblVal;
		else if (op == OpNode::opMoreEq)
			b = left->DblVal >= right->DblVal;
		else if (op == OpNode::opEq)
			b = left->DblVal == right->DblVal;
		else if (op == OpNode::opNeq)
			b = left->DblVal != right->DblVal;
		else
			ASSERT(0);
		
		return constBool(b);
	}
	else if (e == ass.CDouble) {
		left->PromoteToFloatValue(ass);
		right->PromoteToFloatValue(ass);
		if (op == OpNode::opLess)
			b = left->DblVal < right->DblVal;
		else if (op == OpNode::opLessEq)
			b = left->DblVal <= right->DblVal;
		else if (op == OpNode::opMore)
			b = left->DblVal > right->DblVal;
		else if (op == OpNode::opMoreEq)
			b = left->DblVal >= right->DblVal;
		else if (op == OpNode::opEq)
			b = left->DblVal == right->DblVal;
		else if (op == OpNode::opNeq)
			b = left->DblVal != right->DblVal;
		else
			ASSERT(0);
		
		return constBool(b);
	}
	else
		ASSERT_(0, ops[op]);
	
	return nullptr;
}

Node* IRGenerator::opBitAndCT(Node* left, Node* right, ZClass* e) {
	if (e == ass.CByte || e == ass.CWord || e == ass.CDWord) {
		uint32 dInt = (uint32)left->IntVal & (uint32)right->IntVal;
		if (FoldConstants)	{
			Node* fold = constIntSigned(dInt);
			fold->Class = ass.CDWord;
			return fold;
		}
	}
	else if (e == ass.CSmall || e == ass.CShort || e == ass.CInt) {
		int dInt = (int32)left->IntVal & (int32)right->IntVal;
		if (FoldConstants)
			return constIntSigned(dInt);
	}
	else
		ASSERT_(0, "bitand");
	
	return nullptr;
}

Node* IRGenerator::opBitOrCT(Node* left, Node* right, ZClass* e) {
	if (e == ass.CByte || e == ass.CWord || e == ass.CDWord) {
		uint32 dInt = (uint32)left->IntVal | (uint32)right->IntVal;
		if (FoldConstants)	{
			Node* fold = constIntSigned(dInt);
			fold->Class = ass.CDWord;
			return fold;
		}
	}
	else if (e == ass.CSmall || e == ass.CShort || e == ass.CInt) {
		int dInt = (int32)left->IntVal | (int32)right->IntVal;
		if (FoldConstants)
			return constIntSigned(dInt);
	}
	else
		ASSERT_(0, "bitor");
		
	return nullptr;
}

Node* IRGenerator::opBitXorCT(Node* left, Node* right, ZClass* e) {
	if (e == ass.CByte || e == ass.CWord || e == ass.CDWord) {
		uint32 dInt = (uint32)left->IntVal ^ (uint32)right->IntVal;
		if (FoldConstants)	{
			Node* fold = constIntSigned(dInt);
			fold->Class = ass.CDWord;
			return fold;
		}
	}
	else if (e == ass.CSmall || e == ass.CShort || e == ass.CInt) {
		int dInt = (int32)left->IntVal ^ (int32)right->IntVal;
		if (FoldConstants)
			return constIntSigned(dInt);
	}
	else
		ASSERT_(0, "bitxor");
		
	return nullptr;
}

Node* IRGenerator::opLogCT(Node* left, Node* right, OpNode::Type op) {
	bool cst = left->IsCT && right->IsCT;
	bool b;

	if (cst) {
		if (op == OpNode::opLogAnd)
			b = (bool)left->IntVal && (bool)right->IntVal;
		else if (op == OpNode::opLogOr)
			b = (bool)left->IntVal || (bool)right->IntVal;
		else
			ASSERT(0);
		
		return constBool(b);
	}
	else {
		if (op == OpNode::opLogAnd && ((left->IsCT && left->IntVal == 0) || (right->IsCT && right->IntVal == 0))) {
			return constBool(false);
		}
		else if (op == OpNode::opLogOr && ((left->IsCT && left->IntVal == 1) || (right->IsCT && right->IntVal == 1))) {
			return constBool(true);
		}
	}

	return nullptr;
}
	
}