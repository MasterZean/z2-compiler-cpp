#include "CppNodeWalker.h"

namespace Z2 {
	
void CppNodeWalker::Walk(Node* node) {
	ASSERT_(node, "Null node");
	if (node->NT == NodeType::Const)
		Walk(*(ConstNode*)node);
	/*else if (node->NT == NodeType::BinaryOp)
		Walk((OpNode*)node);
	else if (node->NT == NodeType::UnaryOp)
		Walk((UnaryOpNode*)node);
	else if (node->NT == NodeType::Memory)
		Walk((MemNode*)node);
	else if (node->NT == NodeType::Cast)
		Walk((CastNode*)node);
	else if (node->NT == NodeType::Temporary)
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
		Walk(*(VarNode*)node);
	/*else if (node->NT == NodeType::Alloc)
		Walk((AllocNode*)node);
	else if (node->NT == NodeType::Array)
		Walk((RawArrayNode*)node);
	else if (node->NT == NodeType::Using)
		Walk((UsingNode*)node);*/
	else
		ASSERT_(0, "Invalid node");
}


void CppNodeWalker::Walk(ConstNode& node) {
	if (node.Class == ass.CQWord) {
		if (IsNull(node.IntVal))
			stream << "9223372036854775808ull";
		else
			stream << Format64((uint64)node.IntVal) << "ull";
	}
	else if (node.Class == ass.CLong) {
		if (IsNull(node.IntVal))
			stream << "-9223372036854775808ll";
		else
			stream << IntStr64(node.IntVal) << "ll";
	}
	else if (node.Class == ass.CDWord || node.Class == ass.CWord || node.Class == ass.CByte) {
		if (node.Base == 16)
			stream << "0x" << Format64Hex((uint32)node.IntVal) << 'u';
		else
			stream << Format64((uint32)node.IntVal) << 'u';
	}
	else if (node.Class == ass.CPtrSize) {
		if (node.Base == 16)
			stream << "0x" << Format64Hex(node.IntVal) << "u";
		else
			stream << Format64(node.IntVal) << "u";
	}
	else if (node.Class == ass.CInt || node.Class == ass.CSmall || node.Class == ass.CShort) {
		if (node.Base == 16) {
			if (node.IntVal == -2147483648ll)
				stream << "(int32)" << "0x" << ToUpper(Format64Hex(node.IntVal)) << "ll";
			else
				stream << "0x" << ToUpper(Format64Hex(node.IntVal));
		} else {
			if (node.IntVal == -2147483648ll)
				stream << "(int32)" << IntStr64(node.IntVal) << "ll";
			else
				stream << IntStr64(node.IntVal);
		}
	}
	else if (node.Class == ass.CFloat) {
		if (node.DblVal != node.DblVal)
			stream << "((float)(1e+300 * 1e+300) * 0.0f)";
		else if (node.DblVal == (float)(1e+300 * 1e+300))
			stream << "((float)(1e+300 * 1e+300))";
		else if (node.DblVal == -(float)(1e+300 * 1e+300))
			stream << "(-(float)(1e+300 * 1e+300))";
		else {
			if (IsNull(node.DblVal))
				stream << "-1.79769e+308" << "\f";
			else {
				String s = FormatDouble(node.DblVal);
				stream << s;
				if (s.Find('.') == -1 && s.Find('e') == -1)
				    stream << ".0";
				stream << 'f';
			}
		}
	}
	else if (node.Class == ass.CDouble) {
		if (node.DblVal != node.DblVal)
			stream << "((float)(1e+300 * 1e+300) * 0.0f)";
		else if (node.DblVal == (float)(1e+300 * 1e+300))
			stream << "((float)(1e+300 * 1e+300))";
		else if (node.DblVal == -(float)(1e+300 * 1e+300))
			stream << "(-(float)(1e+300 * 1e+300))";
		else {
			if (IsNull(node.DblVal))
				stream << "-1.79769e+308";
			else {
				String s = FormatDouble(node.DblVal);
				stream << s;
				if (s.Find('.') == -1 && s.Find('e') == -1)
					stream << ".0";
			}
		}
	}
	else if (node.Class == ass.CBool) {
		if (node.IntVal == 0)
			stream << "false";
		else
			stream << "true";
	}
	else if (node.Class == ass.CChar) {
		if (node.IntVal >= 127) {
			if (node.Base == 16)
				stream << "0x" << Format64Hex(node.IntVal);
			else
				stream << node.IntVal;
			return;
		}
		
		stream << '\'';
		
		if (node.IntVal == '\t')
			stream << "\\t";
		else if (node.IntVal == '\n')
			stream << "\\n";
		else if (node.IntVal == '\r')
			stream << "\\r";
		else if (node.IntVal == '\a')
			stream << "\\a";
		else if (node.IntVal == '\b')
			stream << "\\b";
		else if (node.IntVal == '\f')
			stream << "\\f";
		else if (node.IntVal == '\v')
			stream << "\\v";
		else if (node.IntVal == '\'')
			stream << "\\\'";
		else if (node.IntVal == '\\')
			stream << "\\\\";
		else if (node.IntVal == '\0')
			stream << "\\0";
		else
			stream << (char)node.IntVal;
		
		stream << '\'';
	}
	/*else if (node.Class == ass.CCls) {
		ZClass& ccc = ass.Classes[(int)node.IntVal];
		stream << ass.CCls->NamespaceQual;
		stream << "Class(" << ccc.RTTIIndex << ")";
	}
	else if (node.Class == ass.CNull)
		stream << "nullptr";
	else if (node.Class == ass.CString)
		stream << "S_[" << (int)node.IntVal << ']';
	else if (node.Class->Scan.IsEnum) {
		ZClass& ec = *node.Class;
		
		for (int i = 0; i < ec.Constants.GetCount(); i++)
			if (node.IntVal == i) {
				stream << ec.NamespaceQual << ec.BackendName << "::" << ec.Constants[i].Name;
				return;
			}
			
		stream << IntStr64(node.IntVal);
	}*/
	else
		ASSERT_(0, "Invalid const node");
}

void CppNodeWalker::Walk(VarNode& node) {
	Variable& var = *node.Var;
	ASSERT(var.Value);
	
	stream << var.Value->Class->BackendName << " " << var.Name << " = ";
	Walk(var.Value);
}

}