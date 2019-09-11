#include "IRGenerator.h"
#include "Assembly.h"

namespace Z2 {
	
int tabAdd[][14] = {
	              /*   b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64,  f80, c,   p */
	/*  0: Bool    */ { 1,  1,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  1: Small   */ { 1,  1,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  2: Byte    */ { 2,  2,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  3: Short   */ { 3,  3,  3,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  4: Word    */ { 4,  4,  4,  4,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  5: Int     */ { 5,  5,  5,  5,   5,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  6: DWord   */ { 6,  6,  6,  6,   6,   6,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  7: Long    */ { 7,  7,  7,  7,   7,   7,   7,   7,   8,   9,   10,  11,  12,  13 },
	/*  8: QWord   */ { 8,  8,  8,  8,   8,   8,   8,   8,   8,   9,   10,  11,  12,  13 },
	/*  9: Float   */ { 9,  9,  9,  9,   9,   9,   9,   9,   9,   9,   10,  11,  -1,  -1 },
	/* 10: Double  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10,  10,  10,  11,  -1,  -1 },
	/* 11: Real80  */ { 11, 11, 11, 11,  11,  11,  11,  11,  11,  11,  11,  11,  -1,  -1 },
	/* 12: Char    */ { 12, 12, 12, 12,  12,  12,  12,  12,  12,  -1,  -1,  -1,  12,  -1 },
	/* 13: PtrSize */ { 13, 13, 13, 13,  13,  13,  13,  13,  13,  -1,  -1,  -1,  -1,  13 },
};

int tabRel[][14] = {
	              /*   b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64,  f80, c,   p */
	/*  0: Bool    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  1: Small   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  2: Byte    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  3: Short   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  4: Word    */ { 4,  4,  4,  4,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  5: Int     */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  6: DWord   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  7: Long    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  8: QWord   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  9: Float   */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 10: Double  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 11: Real80  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 12: Char    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/* 13: PtrSize */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
};

int tabSft[][14] = {
	              /*    b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64, f80, c,   p */
	/*  0: Bool    */ { -1, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
	/*  1: Small   */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  2: Byte    */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  3: Short   */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  4: Word    */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  5: Int     */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  6: DWord   */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  7: Long    */ { -1,  7,  7,  7,   7,   7,   7,   7,   7,  -1,  -1,  -1,  -1,   7 },
	/*  8: QWord   */ { -1,  8,  8,  8,   8,   8,   8,   8,   8,  -1,  -1,  -1,  -1,   8 },
	/*  9: Float   */ { -1,  9,  9,  9,   9,   9,   9,   9,   9,  -1,  -1,  -1,  -1,   9 },
	/* 10: Double  */ { -1, 10, 10, 10,  10,  10,  10,  10,  10,  -1,  -1,  -1,  -1,  10 },
	/* 11: Real80  */ { -1, 11, 11, 11,  11,  11,  11,  11,  11,  -1,  -1,  -1,  -1,  11 },
	/* 12: Char    */ { -1, 12, 12, 12,  12,  12,  12,  12,  12,  -1,  -1,  -1,  -1,  12 },
	/* 13: PtrSize */ { -1, 13, 13, 13,  13,  13,  13,  13,  13,  -1,  -1,  -1,  -1,  13 },
};
	
void IRGenerator::fillSignedTypeInfo(int64 l, Node* node, ZClass* cls) {
	node->Class = ass.CInt;

	if (l >= -128 && l <= 127) {
		node->C1 = ass.CSmall;
		if (l >= 0)
			node->C2 = ass.CByte;
	}
	else if (l >= 0 && l <= 255) {
		node->C1 = ass.CShort;
		node->C2 = ass.CByte;
	}
	else if (l >= -32768 && l <= 32767) {
		node->C1 = ass.CShort;
		if (l >= 0)
			node->C2 = ass.CWord;
	}
	else if (l >= 0 && l <= 65535) {
		node->C1 = ass.CInt;
		node->C2 = ass.CWord;
	}
	else if (l >= -2147483648LL && l <= 2147483647) {
		node->C1 = ass.CInt;
		if (l >= 0)
			node->C2 = ass.CDWord;
	}
	else {
		node->Class = ass.CLong;
		node->C1 = ass.CLong;
		if (l >= 0 && l <= 4294967295)
			node->C2 = ass.CDWord;
		else if (l >= 0 && l <= 9223372036854775807ul)
			node->C2 = ass.CQWord;
	}
	
	if (cls)
		node->Class = cls;
}

void IRGenerator::fillUnsignedTypeInfo(uint64 l, Node* node, ZClass* cls) {
	node->Class = ass.CDWord;
	node->C1 = ass.CDWord;
	if (l <= 127) {
		node->C1 = ass.CByte;
		node->C2 = ass.CSmall;
	}
	else if (l <= 255)
		node->C1 = ass.CByte;
	else if (l <= 32767) {
		node->C1 = ass.CWord;
		node->C2 = ass.CShort;
	}
	else if (l <= 65535)
		node->C1 = ass.CWord;
	else if (l <= 2147483647)
		node->C2 = ass.CInt;
	else if (l <= 4294967295) {
	}
	else if (l <= 9223372036854775807ul) {
		node->Class = ass.CQWord;
		node->C1 = ass.CQWord;
		node->C2 = ass.CLong;
	}
	else {
		node->Class = ass.CQWord;
		node->C1 = ass.CQWord;
	}
	
	if (cls)
		node->Class = cls;
}

ConstNode* IRGenerator::constIntSigned(int64 l, int base, ZClass* cls) {
	ConstNode* node = constNodes.Get();
	
	fillSignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsTemporary = true;
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constIntUnsigned(uint64 l, int base, ZClass* cls) {
	ConstNode* node = constNodes.Get();
	
	fillUnsignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsTemporary = true;
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constFloatSingle(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CFloat);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constFloatDouble(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CDouble);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

VarNode* IRGenerator::defineLocalVar(Variable& v) {
	VarNode* var = varNodes.Get();
	
	ASSERT(v.Value);
	
	var->Var = &v;
	var->SetType(v.Value->Class);
	var->HasSe = true;
	
	var->IsAddressable = true;
	
	ASSERT(var->Class);
	
	return var;
}

MemNode* IRGenerator::mem(Variable& v) {
	MemNode* var = memNodes.Get();
	
	var->Var = &v;
	var->SetType(v.Value->Class);
	var->HasSe = true;
	
	var->IsAddressable = true;
	
	ASSERT(var->Class);
	
	return var;
}

BlockNode* IRGenerator::openBlock() {
	BlockNode* block = blockNodes.Get();
	block->IntVal = 1;
	return block;
}

BlockNode* IRGenerator::closeBlock() {
	BlockNode* block = blockNodes.Get();
	return block;
}

Node* IRGenerator::op(Node* left, Node* right, OpNode::Type op, const Point& p) {
	if (op <= OpNode::opMod)
		return opArit(left, right, op, p);
	/*else if (op <= OpNode::opShl)
		return shl(left, right, p);
	else if (op <= OpNode::opShr)
		return shr(left, right, p);
	else if (op <= OpNode::opNeq)
		return opRel(left, right, op, p);
	else if (op <= OpNode::opBitAnd)
		return op_bitand(left, right);
	else if (op <= OpNode::opBitXor)
		return op_bitxor(left, right);
	else if (op <= OpNode::opBitOr)
		return op_bitor(left, right);
	else if (op <= OpNode::opLogOr)
		return opLog(left, right, op);*/
	else {
		ASSERT(0);
		return nullptr;
	}
}

Node* IRGenerator::opArit(Node* left, Node* right, OpNode::Type op, const Point& p) {
	bool valid = left->Class->MIsNumeric && right->Class->MIsNumeric;
	if (!valid)
		return nullptr;
	
	int t1 = left->Class->MIndex;
	int t2 = right->Class->MIndex;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	
	int t = tabAdd[t1][t2];
	if (t == -1)
		return nullptr;

	ZClass* cls = &ass.Classes[t];
	ZClass* e1 = 0;
	ZClass* e2 = 0;
	
	if (left->Class != right->Class) {
		if (left->Class == right->C1)
			e2 = right->C1;
		if (left->Class == right->C2)
			e2 = right->C2;
		
		if (right->Class == left->C1)
			e2 = left->C1;
		if (right->Class == left->C2)
			e2 = left->C2;
	}
	
	e1 = cls;
	
	OpNode* node = opNodes.Get();

	if (left->C1 == ass.CBool && right->C1 == ass.CBool && e1 == ass.CSmall)
		e2 = ass.CByte;

	node->OpA = left;
	node->OpB = right;
	node->Op = op;

	node->IsCT = left->IsCT && right->IsCT;
	node->SetType(cls, e1, e2);
	//node->IntVal = dInt;
	//node->DblVal = dDouble;

	ASSERT(node->Class);
	
	return node;
}

}
