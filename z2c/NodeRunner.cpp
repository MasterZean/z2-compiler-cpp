#include "NodeRunner.h"
#include "tables.h"

namespace Z2 {
	
Node* NodeRunner::Execute(Node* node) {
	ASSERT_(node, "Null node");
		
	if (node->NT == NodeType::Const)
		return ExecuteNode(*(ConstNode*)node);
	else if (node->NT == NodeType::BinaryOp)
		return ExecuteNode(*(OpNode*)node);
	/*else if (node->NT == NodeType::UnaryOp)
		Walk((UnaryOpNode*)node);*/
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
	/*else if (node->NT == NodeType::List)
		Walk((ListNode*)node);
	else if (node->NT == NodeType::Construct)
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
		stream << "// call " << over.OwnerMethod.Name;
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
					WriteValue(var->Var->Value);
				}
				else if (node->Class != ass.CVoid)
					WriteValue(node);
				NL();
			}
		}
	}
	
	if (CallDepth >= StartCallDepth) {
		stream << "// return " << over.OwnerMethod.Name;
		NL();
	}
	
	CallDepth--;
	
	return ret;
}

void NodeRunner::WriteValue(Node* node) {
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
	stream << "}";
}

Node* NodeRunner::ExecuteNode(ConstNode& node) {
	DUMP(node.IntVal);
	return &node;
}

Node* NodeRunner::ExecuteNode(BlockNode& node) {
	return nullptr;
}

Node* NodeRunner::ExecuteNode(VarNode& node) {
	DUMP(node.Var->Name);
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
		if (node.Class == ass.CByte || node.Class == ass.CWord || node.Class == ass.CDWord || node.Class == ass.CQWord) {
			uint64 dInt = (uint64)left->IntVal >> (uint64)right->IntVal;
			return irg.constIntUnsigned(dInt, node.Class);
		}
		else if (node.Class == ass.CSmall || node.Class == ass.CShort || node.Class == ass.CInt || node.Class == ass.CLong) {
			int64 dInt = (int64)left->IntVal >> (int64)right->IntVal;
			return irg.constIntSigned(dInt, node.Class);
		}
		else
			ASSERT_(0, "shl");
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

Node* NodeRunner::ExecuteNode(MemNode& node) {
	DUMP(node.Var->Value);
	return Execute(node.Var->Value);
}

Node* NodeRunner::ExecuteNode(CastNode& node) {
	Execute(node.Object);
	
	node.IntVal = node.Object->IntVal;
	node.DblVal = node.Object->DblVal;
	
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
	
	return &node;
}

Node* NodeRunner::ExecuteNode(CallNode& node) {
	return ExecuteOverload(*node.Over);
}

Node* NodeRunner::ExecuteNode(RetNode& node) {
	if (node.Value)
		return Execute(node.Value);
	
	return nullptr;
}

Node* NodeRunner::ExecuteNode(AssignNode& node) {
	//Execute(node.LS);
	if (node.LS->NT == NodeType::Memory) {
		MemNode& var = (MemNode&)node;
		
		if (node.Op == OpNode::opNotSet)
			var.Var->Value = Execute(node.RS);
		else
			var.Var->Value = Execute(irg.op(Execute(node.LS), Execute(node.RS), node.Op, Point(0, 0)));
		
		return var.Var->Value;
	}
	
	return nullptr;
}

}

