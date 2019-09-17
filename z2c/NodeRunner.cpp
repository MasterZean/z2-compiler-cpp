#include "NodeRunner.h"

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
	else if (node->NT == NodeType::Cast)
		return ExecuteNode(*(CastNode*)node);
	/*else if (node->NT == NodeType::Temporary)
		Walk((TempNode*)node);
	else if (node->NT == NodeType::Def)
		Walk((DefNode*)node);
	else if (node->NT == NodeType::List)
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
		Walk((IntNode*)node);
	else if (node->NT == NodeType::Return)
		Walk((ReturnNode*)node);*/
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

void NodeRunner::Execute(Overload& over) {
	for (int i = 0; i < over.Nodes.GetCount(); i++) {
		Node* node = Execute(over.Nodes[i]);
		if (node) {
			if (over.Nodes[i]->NT == NodeType::Var) {
				VarNode* var = (VarNode*)over.Nodes[i];
				stream << "var " << var->Var->Name << ": ";
				stream << var->Var->Class->Name << " = ";
				WriteValue(var->Var->Value);
			}
			else
				WriteValue(node);
		}
		stream << "\r\n";
	}
}

void NodeRunner::WriteValue(Node* node) {
	stream << node->Class->Name << "{";
	if (node->Class->MIsNumeric) {
		if (node->Class == ass.CFloat || node->Class == ass.CDouble)
			stream << node->DblVal;
		else
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
	Node* l = Execute(node.OpA);
	Node* r = Execute(node.OpB);
	
	int64 dInt;
	double dDouble;
	
	return irg.opAritCT(l, r, node.Op, l->Class, l->Class, dInt, dDouble);
}

Node* NodeRunner::ExecuteNode(MemNode& node) {
	return Execute(node.Var->Value);
}

Node* NodeRunner::ExecuteNode(CastNode& node) {
	return nullptr;
}

}

