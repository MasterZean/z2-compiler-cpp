#include "NodeRunner.h"
#include "tables.h"

namespace Z2 {
	
Node* NodeRunner::Execute(Node* node) {
	ASSERT_(node, "Null node");
		
	if (node->NT == NodeType::Const)
		return ExecuteNode(*(ConstNode*)node);
	else if (node->NT == NodeType::BinaryOp)
		return ExecuteNode(*(OpNode*)node);
	else if (node->NT == NodeType::UnaryOp)
		return ExecuteNode(*(UnaryOpNode*)node);
	else if (node->NT == NodeType::Memory)
		return ExecuteNode(*(MemNode*)node);
	else if (node->NT == NodeType::Assign)
		return ExecuteNode(*(AssignNode*)node);
	else if (node->NT == NodeType::Cast)
		return ExecuteNode(*(CastNode*)node);
	/*else if (node->NT == NodeType::Temporary)
		Walk((TempNode*)node);*/
	else if (node->NT == NodeType::Call)
		return ExecuteNode(*(CallNode*)node);
	else if (node->NT == NodeType::List)
		return ExecuteNode(*(ListNode*)node);
	/*else if (node->NT == NodeType::Construct)
		Walk((ConstructNode*)node);
	else if (node->NT == NodeType::Ptr)
		Walk((PtrNode*)node);
	else if (node->NT == NodeType::Index)
		Walk((IndexNode*)node);
	else if (node->NT == NodeType::SizeOf)
		Walk((SizeOfNode*)node);
	else if (node->NT == NodeType::Property)
		Walk((PropertyNode*)node);
	else if (node->NT == NodeType::Deref)
		Walk((DerefNode*)node);
	else if (node->NT == NodeType::Intrinsic)
		Walk((IntNode*)node);*/
	else if (node->NT == NodeType::Return)
		return ExecuteNode(*(RetNode*)node);
	else if (node->NT == NodeType::Var)
		return ExecuteNode(*(VarNode*)node);
	/*else if (node->NT == NodeType::Alloc)
		Walk((AllocNode*)node);
	else if (node->NT == NodeType::Array)
		Walk((RawArrayNode*)node);
	else if (node->NT == NodeType::Using)
		Walk((UsingNode*)node);*/
	else if (node->NT == NodeType::Block)
		return ExecuteNode(*(BlockNode*)node);
	else
		ASSERT_(0, "Invalid node");
	
	return nullptr;
}

Node* NodeRunner::ExecuteOverload(Overload& over) {
	CallDepth++;
	
	if (CallDepth >= StartCallDepth) {
		StringStream p;
		for (int i = 0; i < over.Params.GetCount(); i++) {
			if (i)
				p << ", ";
			
			p << over.Params[i].Class->Name << " = ";
			
			if ((*paramList)[i]->NT == NodeType::Const)
				WriteValue(p, (*paramList)[i]);
			else
				p << "*";
		}
		
		String ps = p;
		
		stream << "// call " << over.OwnerMethod.Name << "(" << ps << ")";
		LOG(String().Cat() << "call " << over.OwnerMethod.Name << "(" << ps << ")");
		NL();
	}
	
	Node* ret = nullptr;
	
	for (int i = 0; i < over.Nodes.GetCount(); i++) {
		Node* in = over.Nodes[i];
		if (in->NT == NodeType::Block) {
			if (in->IntVal == 1) {
				SS();
				stream << "{";
				NL();
				indent++;
			}
			else {
				indent--;
				SS();
				stream << "}";
				NL();
			}
		}
		else if (in->NT == NodeType::Return) {
			ret = Execute(in);
			break;
		}
		else {
			SS();
			
			Node* node = Execute(in);
			if (node) {
				if (in->NT == NodeType::Var) {
					VarNode* var = (VarNode*)in;
					stream << "var " << var->Var->Name << ": ";
					stream << var->Var->Class->Name << " = ";
					WriteValue(stream, var->Var->Value);
				}
				else if (node->Class != ass.CVoid)
					WriteValue(stream, node);
				NL();
			}
		}
	}
	
	if (CallDepth >= StartCallDepth) {
		stream << "// return " << over.OwnerMethod.Name << "(" << over.Signature << ")";
		NL();
	}
	
	CallDepth--;
	
	return ret;
}

void NodeRunner::WriteValue(Stream& stream, Node* node) {
	stream << node->Class->Name << "{";
	
	if (node->Class->MIsNumeric) {
		if (node->Class == ass.CFloat || node->Class == ass.CDouble)
			stream << node->DblVal;
		else if (node->Class == ass.CSmall)
			stream << (int)(int8)node->IntVal;
		else if (node->Class == ass.CByte)
			stream << (int)(uint8)node->IntVal;
		else if (node->Class == ass.CShort)
			stream << (int)(int16)node->IntVal;
		else if (node->Class == ass.CWord)
			stream << (int)(uint16)node->IntVal;
		else if (node->Class == ass.CInt)
			stream << (int32)node->IntVal;
		else if (node->Class == ass.CDWord)
			stream << (uint32)node->IntVal;
		else if (node->Class == ass.CLong) {
			if (IsNull(node->IntVal))
				stream << "-9223372036854775808";
			else
				stream << (int64)node->IntVal;
		}
		else if (node->Class == ass.CQWord) {
			if (IsNull(node->IntVal))
				stream << "18446744073709551615";
			else
				stream << (uint64)node->IntVal;
		}
		else if (node->Class == ass.CBool) {
			if (node->IntVal == 0)
				stream << "false";
			else
				stream << "true";
		}
		else
			stream << node->IntVal;
	}
	else if (node->Class == ass.CCls) {
		stream << node->IntVal;
	}
	
	stream << "}";
}

Node* NodeRunner::ExecuteNode(ConstNode& node) {
	return &node;
}

Node* NodeRunner::ExecuteNode(BlockNode& node) {
	return nullptr;
}

Node* NodeRunner::ExecuteNode(VarNode& node) {
	node.Var->Value = Execute(node.Var->Value);
	return node.Var->Value;
}

Node* NodeRunner::ExecuteNode(OpNode& node) {
	Node* left = Execute(node.OpA);
	Node* right = Execute(node.OpB);
	
	if (node.Op <= OpNode::opMod) {
		int64 dInt;
		double dDouble;
		
		return irg.opAritCT(left, right, node.Op, node.Class, node.C1, dInt, dDouble);
	}
	else if (node.Op <= OpNode::opShl) {
		if (node.Class == ass.CByte || node.Class == ass.CWord || node.Class == ass.CDWord || node.Class == ass.CQWord) {
			uint64 dInt = (uint64)left->IntVal << (uint64)right->IntVal;
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else if (node.Class == ass.CSmall || node.Class == ass.CShort || node.Class == ass.CInt || node.Class == ass.CLong) {
			int64 dInt = (int64)left->IntVal << (int64)right->IntVal;
			return irg.constIntSigned(dInt, node.Class);
		}
		else
			ASSERT_(0, "shl");
	}
	else if (node.Op <= OpNode::opShr) {
		if (node.Class == ass.CSmall) {
			int64 dInt = (int8)((int8)left->IntVal >> (int64)right->IntVal);
			return irg.constIntSigned(dInt, node.Class);
		}
		else if (node.Class == ass.CByte) {
			uint64 dInt = (uint8)((uint8)left->IntVal >> (int64)right->IntVal);
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else if (node.Class == ass.CShort) {
			int64 dInt = (int16)((int16)left->IntVal >> (int64)right->IntVal);
			return irg.constIntSigned(dInt, node.Class);
		}
		else if (node.Class == ass.CWord) {
			uint64 dInt = (uint16)((uint16)left->IntVal >> (int64)right->IntVal);
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else if (node.Class == ass.CInt) {
			int64 dInt = (int32)((int32)left->IntVal >> (int64)right->IntVal);
			return irg.constIntSigned(dInt, node.Class);
		}
		else if (node.Class == ass.CDWord) {
			uint64 dInt = (uint32)((uint32)left->IntVal >> (int64)right->IntVal);
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else if (node.Class == ass.CLong) {
			int64 dInt = (int64)((int64)left->IntVal >> (int64)right->IntVal);
			return irg.constIntSigned(dInt, node.Class);
		}
		else if (node.Class == ass.CQWord) {
			uint64 dInt = (uint64)((uint64)left->IntVal >> (int64)right->IntVal);
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else
			ASSERT_(0, "shr");
	}
	else if (node.Op <= OpNode::opNeq) {
		int t = TabRel[left->Class->MIndex][right->Class->MIndex];
		
		return irg.opRelCT(left, right, node.Op, &ass.Classes[t]);
	}
	else if (node.Op <= OpNode::opBitAnd)
		return irg.opBitAndCT(left, right, node.Class);
	else if (node.Op <= OpNode::opBitXor)
		return irg.opBitOrCT(left, right, node.Class);
	else if (node.Op <= OpNode::opBitOr)
		return irg.opBitXorCT(left, right, node.Class);
	else if (node.Op <= OpNode::opLogOr)
		return irg.opLogCT(left, right, node.Op);
	else
		ASSERT(0);
	
	return nullptr;
}

union CpuValue {
	int8 i8;
	uint8 u8;
	int16 i16;
	uint16 u16;
	int32 i32;
	uint32 u32;
	int64 i64;
	uint64 u64;
	double f64;
	//long double f80;
	void* ptr;
	
	void negate(int t);
	void bitnot(int t);
};

CpuValue NodeToVal(const Node& n) {
	CpuValue v;
	
	if (n.Class->MIsFloat)
		v.f64 = n.DblVal;
	else
		v.i64 = n.IntVal;

	return v;
}

void CpuValue::negate(int t) {
#ifdef _DEBUG
	int64 temp = i64;
#endif

	i64 = -i64;
	
#ifdef _DEBUG
	if (t == 1) {
		int8 v = temp;
		v = -v;
		
		ASSERT(((int8)i64) == v);
		ASSERT(i8 == v);
	}
	else if (t == 2) {
		uint8 v = temp;
		v = -v;
		
		ASSERT(((uint8)i64) == v);
		ASSERT(u8 == v);
	}
	else if (t == 3) {
		int16 v = temp;
		v = -v;
		
		ASSERT(((int16)i64) == v);
		ASSERT(i16 == v);
	}
	else if (t == 4) {
		uint16 v = temp;
		v = -v;
		
		ASSERT(((uint16)i64) == v);
		ASSERT(u16 == v);
	}
	else if (t == 5) {
		int32 v = temp;
		v = -v;
		
		ASSERT(((int32)i64) == v);
		ASSERT(i32 == v);
	}
	else if (t == 6) {
		uint32 v = temp;
		v = -v;

		ASSERT(((uint32)i64) == v);
		ASSERT(u32 == v);
	}
	else if (t == 7) {
		int64 v = temp;
		v = -v;
		
		ASSERT(i64 == v);
	}
	else if (t == 8) {
		uint64 v = temp;
		v = -v;

		ASSERT(u64 == v);
	}
#endif
}

void CpuValue::bitnot(int t) {
#ifdef _DEBUG
	int64 temp = i64;
#endif

	i64 = ~i64;
	
#ifdef _DEBUG
	if (t == 1) {
		int8 v = temp;
		v = ~v;
		
		ASSERT(((int8)i64) == v);
		ASSERT(i8 == v);
	}
	else if (t == 2) {
		uint8 v = temp;
		v = ~v;
		
		ASSERT(((uint8)i64) == v);
		ASSERT(u8 == v);
	}
	else if (t == 3) {
		int16 v = temp;
		v = ~v;
		
		ASSERT(((int16)i64) == v);
		ASSERT(i16 == v);
	}
	else if (t == 4) {
		uint16 v = temp;
		v = ~v;
		
		ASSERT(((uint16)i64) == v);
		ASSERT(u16 == v);
	}
	else if (t == 5) {
		int32 v = temp;
		v = ~v;
		
		ASSERT(((int32)i64) == v);
		ASSERT(i32 == v);
	}
	else if (t == 6) {
		uint32 v = temp;
		v = ~v;

		ASSERT(((uint32)i64) == v);
		ASSERT(u32 == v);
	}
	else if (t == 7) {
		int64 v = temp;
		v = ~v;
		
		ASSERT(i64 == v);
	}
	else if (t == 8) {
		uint64 v = temp;
		v = ~v;

		ASSERT(u64 == v);
	}
#endif
}

Node* NodeRunner::ExecuteNode(UnaryOpNode& node) {
	if (node.Op == OpNode::opMinus) {
		CpuValue v = NodeToVal(*Execute(node.OpA));
		v.negate(node.OpA->Class->MIndex);
		
		return irg.constIntSigned(v.i64, node.OpA->Class);
	}
	else if (node.Op == OpNode::opBitNot) {
		CpuValue v = NodeToVal(*Execute(node.OpA));
		v.bitnot(node.OpA->Class->MIndex);
		
		return irg.constIntSigned(v.i64, node.OpA->Class);
	}
	else if (node.Op == OpNode::opNot) {
		CpuValue v = NodeToVal(*Execute(node.OpA));
		
		return irg.constBool(v.i64 == 0);
	}
	else
		ASSERT(0);
	
	return nullptr;
}

Node* NodeRunner::ExecuteNode(MemNode& node) {
	if (node.Var->MIsParam >= 0) {
		ASSERT(paramList);
		return (*paramList)[node.Var->MIsParam];
	}
	else
		return Execute(node.Var->Value);
}

Node* NodeRunner::ExecuteNode(CastNode& node) {
	Node* res = Execute(node.Object);
	
	node.IntVal = res->IntVal;
	node.DblVal = res->DblVal;
	
	if (node.Class == ass.CFloat) {
		if (node.Object->Class != ass.CFloat && node.Object->Class != ass.CDouble) {
			node.DblVal = node.IntVal;
		}
	}
	else if (node.Class == ass.CDouble) {
		if (node.Object->Class != ass.CFloat && node.Object->Class != ass.CDouble) {
			node.DblVal = node.IntVal;
		}
	}
	else if (node.Class == ass.CBool) {
		if (node.Object->Class != ass.CFloat && node.Object->Class != ass.CDouble) {
			node.IntVal = (node.IntVal != 0);
		}
		else {
			node.IntVal = (node.DblVal != 0);
		}
	}
	else if (node.Class == ass.CSmall) {
		if (node.Class->MIsFloat)
			node.IntVal = (int64)(int8)node.DblVal;
		else
			node.IntVal = (int64)(int8)node.IntVal;
	}
	else if (node.Class == ass.CShort) {
		if (node.Class->MIsFloat)
			node.IntVal = (int64)(int16)node.DblVal;
		else
			node.IntVal = (int64)(int16)node.IntVal;
	}
	else if (node.Class == ass.CInt) {
		if (node.Class->MIsFloat)
			node.IntVal = (int64)(int32)node.DblVal;
		else
			node.IntVal = (int64)(int32)node.IntVal;
	}
	else if (node.Class == ass.CByte) {
		if (node.Class->MIsFloat)
			node.IntVal = (uint64)(uint8)node.DblVal;
		else
			node.IntVal = (uint64)(uint8)node.IntVal;
	}
	else if (node.Class == ass.CShort) {
		if (node.Class->MIsFloat)
			node.IntVal = (uint64)(uint16)node.DblVal;
		else
			node.IntVal = (uint64)(uint16)node.IntVal;
	}
	else if (node.Class == ass.CInt) {
		if (node.Class->MIsFloat)
			node.IntVal = (uint64)(uint32)node.DblVal;
		else
			node.IntVal = (uint64)(uint32)node.IntVal;
	}
	
	return &node;
}

Node* NodeRunner::ExecuteNode(CallNode& node) {
	Vector<Node*> newParams;
	newParams.Reserve(node.Params.GetCount());
	
	for (int i = 0; i < node.Params.GetCount(); i++)
		newParams << Execute(node.Params[i]);
	
	auto temp = paramList;
	paramList = &newParams;
	
	Node* ret = ExecuteOverload(*node.Over);
	
	paramList = temp;
	
	return ret;
}

Node* NodeRunner::ExecuteNode(RetNode& node) {
	if (node.Value)
		return Execute(node.Value);
	
	return nullptr;
}

Node* NodeRunner::ExecuteNode(AssignNode& node) {
	if (node.LS->NT == NodeType::Memory) {
		Variable& var = *((MemNode&)*node.LS).Var;
		Node* l = Execute(node.LS);
		if (node.Op == OpNode::opNotSet)
			var.Value = Execute(node.RS);
		else
			var.Value = Execute(irg.op(Execute(node.LS), Execute(node.RS), node.Op, Point(0, 0)));
		
		return var.Value;
	}
	
	return nullptr;
}

Node* NodeRunner::ExecuteNode(ListNode& node) {
	Node* n = Execute(node.Object);
	return n;
}

}

