#include "CppNodeWalker.h"
#include "tables.h"
#include "ErrorReporter.h"

namespace Z2 {

void CppNodeWalker::Walk(Node* node) {
	ASSERT_(node, "Null node");
	
	if (node->NT == NodeType::Const)
		WalkNode(*(ConstNode*)node);
	else if (node->NT == NodeType::BinaryOp)
		WalkNode(*(OpNode*)node);
	else if (node->NT == NodeType::UnaryOp)
		WalkNode(*(UnaryOpNode*)node);
	else if (node->NT == NodeType::Memory)
		WalkNode(*(MemNode*)node);
	else if (node->NT == NodeType::Cast)
		WalkNode(*(CastNode*)node);
	else if (node->NT == NodeType::Assign)
		WalkNode(*(AssignNode*)node);
	/*else if (node->NT == NodeType::Temporary)
		Walk((TempNode*)node);*/
	else if (node->NT == NodeType::Call)
		WalkNode(*(CallNode*)node);
	else if (node->NT == NodeType::List)
		WalkNode(*(ListNode*)node);
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
		WalkNode(*(RetNode*)node);
	else if (node->NT == NodeType::Var)
		WalkNode(*(VarNode*)node);
	else if (node->NT == NodeType::If)
		WalkNode(*(IfNode*)node);
	/*else if (node->NT == NodeType::Alloc)
		Walk((AllocNode*)node);
	else if (node->NT == NodeType::Array)
		Walk((RawArrayNode*)node);
	else if (node->NT == NodeType::Using)
		Walk((UsingNode*)node);*/
	else if (node->NT == NodeType::Block)
		WalkNode(*(BlockNode*)node);
	else
		ASSERT_(0, "Invalid node");
}

void CppNodeWalker::WalkNode(ConstNode& node) {
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
	else if (node.Class == ass.CSmall) {
		int val = (int8)node.IntVal;
		if (node.Base == 16) {
			if (node.IntVal >= 0) {
				String temp = ToUpper(FormatIntHex(val, 2));
				temp = temp.Mid(temp.GetCount() - 2);
				stream << "0x" << temp;
			}
			else  {
				String temp = ToUpper(FormatIntHex(-val, 2));
				temp = temp.Mid(temp.GetCount() - 2);
				stream << "-0x" << temp;
			}
		}
		else if (node.Base == 8) {
			if (node.IntVal >= 0) {
				String temp = ToUpper(FormatIntOct(val, 3));
				temp = temp.Mid(temp.GetCount() - 3);
				stream << "0" << temp;
			}
			else  {
				String temp = ToUpper(FormatIntOct(-val, 3));
				temp = temp.Mid(temp.GetCount() - 3);
				stream << "-0" << temp;
			}
		}
		else
			stream << IntStr64(node.IntVal);
	}
	else if (node.Class == ass.CShort) {
		if (node.Base == 16) {
			if (node.IntVal >= 0) {
				String temp = ToUpper(FormatIntHex(node.IntVal, 4));
				temp = temp.Mid(temp.GetCount() - 4);
				stream << "0x" << temp;
			}
			else  {
				String temp = ToUpper(FormatIntHex(-node.IntVal, 4));
				temp = temp.Mid(temp.GetCount() - 4);
				stream << "-0x" << temp;
			}
		}
		else if (node.Base == 8) {
			if (node.IntVal >= 0) {
				String temp = ToUpper(FormatIntOct(node.IntVal, 6));
				temp = temp.Mid(temp.GetCount() - 6);
				stream << "0" << temp;
			}
			else  {
				String temp = ToUpper(FormatIntOct(-node.IntVal, 6));
				temp = temp.Mid(temp.GetCount() - 6);
				stream << "-0" << temp;
			}
		}
		else
			stream << IntStr64(node.IntVal);
	}
	else if (node.Class == ass.CInt) {
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
		else if (node.IntVal < 16)
			stream << "\\x00" << Format64Hex(node.IntVal);
		else if (node.IntVal < 32)
			stream << "\\x0" << Format64Hex(node.IntVal);
		else
			stream << (char)node.IntVal;
		
		stream << '\'';
	}
	else if (node.Class == ass.CCls) {
		ZClass& ccc = ass.Classes[(int)node.IntVal];
		stream << "Class(" << ccc./*RTTIIndex*/MIndex << ")";
	}
	else if (node.Class == ass.CNull)
		stream << "nullptr";
	/*else if (node.Class == ass.CString)
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

void CppNodeWalker::WalkNode(VarNode& node) {
	Variable& var = *node.Var;
	ASSERT(var.Value);
	
	WriteVar(var);
}

void CppNodeWalker::WriteVar(Variable& var) {
	stream << var.Class->BackendName << " " << var.Name << " = ";
	
	if (var.Value->NT == NodeType::Cast) {
		CastNode* cast = (CastNode*)var.Value;
		if (cast->Object->C1 == cast->Class)
			Walk(cast->Object);
		else
			Walk(cast);
	}
	else
		Walk(var.Value);
}

void CppNodeWalker::WalkNode(MemNode& node) {
	Variable& var = *node.Var;
	//ASSERT(var.Value);
	
	if (var.MIsMember)
		stream << "::";
	stream << var.Name;
}

void CppNodeWalker::WalkNode(BlockNode& node) {
	if (node.IntVal) {
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

void CppNodeWalker::WalkNode(OpNode& node) {
	Walk(node.OpA);
	
	stream << ' ' << TabOpString[node.Op] << ' ';

	Walk(node.OpB);
}

void CppNodeWalker::WalkNode(UnaryOpNode& node) {
	if (node.Prefix) {
		stream << TabOpString[node.Op] << "(";
		Walk(node.OpA);
		stream << ")";
	}
	else {
		stream << "(";
		Walk(node.OpA);
		stream << ")" << TabOpString[node.Op];
	}
}


void CppNodeWalker::WalkNode(CastNode& node) {
	stream << "(" << node.Class->BackendName;
	if (node.MoveCast)
		stream << "&&";
	stream << ")(";
	Walk(node.Object);
	stream << ")";
}

void CppNodeWalker::WalkNode(CallNode& node) {
	Method& m = node.Over->OwnerMethod;
	
	stream << "::" << node.Over->BackendName << "(";
	
	int count = m.OverloadCounts[node.Over->Params.GetCount()];
	
	for (int i = 0; i < node.Params.GetCount(); i++) {
		if (i)
			stream << ", ";
		if (count == 0)
			Walk(node.Params[i]);
		else {
			bool proced = false;
			
			if (node.Params[i]->NT == NodeType::Cast) {
				CastNode& c = (CastNode&)*node.Params[i];
				
				if (node.Over->Params[i].Class == node.Params[i]->Class) {
					proced = true;
					c.MoveCast = node.Over->Params[i].PType == Variable::tyMove;
				}
			}
			
			if (!proced) {
				stream << "(" << node.Over->Params[i].Class->BackendName;
				if (node.Over->Params[i].PType == Variable::tyMove)
					stream << "&&";
				stream << ")(";
				Walk(node.Params[i]);
				stream << ")";
			}
			else
				Walk(node.Params[i]);
		}
	}
		
	stream << ")";
}

void CppNodeWalker::WalkNode(RetNode& node) {
	stream << "return";
	
	if (node.Value) {
		stream << " ";
		Walk(node.Value);
	}
}

void CppNodeWalker::WalkNode(IfNode& node) {
	stream << "if (";
	Walk(node.Cond);
	stream << ")";
}

void CppNodeWalker::WalkNode(AssignNode& node) {
	Walk(node.LS);
	
	stream << " ";
	if (node.Op != OpNode::opNotSet)
		stream << TabOpString[node.Op];
	stream << "= ";
	
	Walk(node.RS);
}

void CppNodeWalker::WalkNode(ListNode& node) {
	stream << "(";
	
	for (int i = 0; i < node.Params.GetCount(); i++) {
		if (i)
			stream << ", ";
		Walk(node.Params[i]);
	}
	
	stream << ")";
}

String MangleNamespace(const String& ns) {
	String s;
	
	Vector<String> v = Split(ns, ".");
	for (int i = 0; i < v.GetCount(); i++) {
		if (i)
			s << "_";
		s << v[i];
	}
	
	return s;
}

void CppNodeWalker::WriteOverloadDefinition(Overload &over) {
	// global name
	if (CommentZMethod) {
		stream << "// ";
		stream << over.OwnerClass.Namespace << over.OwnerClass.Name << "::" << over.Name();
		WriteAssemblyParams(stream, over);
		if (over.Return != ass.CVoid)
			stream << over.Return->GlobalName;
		if (over.IsConst)
			stream << "_" << "const";
		NL();
	}
	
	// mangled C
	if (CommentCMangled) {
		stream << "// ";
		stream << "_N";
		stream << MangleNamespace(over.OwnerClass.Namespace) << "_";
		stream << over.OwnerClass.Name;
		stream << "S_";
		stream << "F";
		if (over.IsConst)
			stream << "C";
		stream << "_";
		stream << over.BackendName;
		stream << "_";
		WriteMangledParams(over);
		if (over.Return != ass.CVoid) {
			stream << "_";
			stream << over.Return->MangledName;
		}
		NL();
	}
	
	if (over.IsVirtual)
		stream << "virtual ";
	else if (over.IsInline)
		stream << "inline ";
	
	// TODO: fix
	//else if (over.IsClassCopyCon() || over.IsClassMoveCon() || over.IsClassCopyOperator() || over.IsClassMoveOperator() || over.IsClassEqOperator() || over.IsClassNeqOperator())
	//	cs << "inline ";
	
	//if (cls.CoreSimple || over.IsStatic)
	//	cs << "static ";
	
	if (WriteReturnType(over))
		stream << " ";
	
	WriteOverloadNameParams(over);
}

void CppNodeWalker::WriteOverloadDeclaration(Overload &over) {
	WriteOverloadDefinition(over);
	stream << ";";
	NL();
}

bool CppNodeWalker::WriteReturnType(Overload &over) {
	if (over.IsDestructor == true)
		return false;

	WriteClassName(*over.Return);
	
	return true;
}

void CppNodeWalker::WriteOverloadNameParams(Overload &over) {
	stream << over.BackendName;
	WriteParams(over);
}

void CppNodeWalker::WriteParams(Overload &over) {
	stream << "(";
	
	for (int i = 0; i < over.Params.GetCount(); i++) {
		if (i)
			stream << ", ";
		stream << over.Params[i].Class->BackendName;
		if (over.Params[i].PType == Variable::tyMove)
			stream << "&&";
		stream << " " << over.Params[i].Name;
	}
	
	stream << ")";
}

void CppNodeWalker::WriteAssemblyParams(Stream& s, Overload &over) {
	s << "(";
	
	for (int i = 0; i < over.Params.GetCount(); i++) {
		if (i)
			s << ",";
		s << over.Params[i].Class->GlobalName;
	}
	
	s << ")";
}

void CppNodeWalker::WriteMangledParams(Overload &over) {
	for (int i = 0; i < over.Params.GetCount(); i++) {
		//if (i)
		//	stream << "_B";
		stream << over.Params[i].Class->MangledName;
	}
}

void CppNodeWalker::WriteClassVars(ZClass& cls) {
	for (int i = 0; i < cls.Variables.GetCount(); i++) {
		Variable& var = cls.Variables[i];
		
		stream << "// ";
		stream << cls.Namespace << cls.Name << "::" << var.Name;
		NL();
		
		WriteVar(var);
		stream << ";";
		NL();
	}
	
	if (cls.Variables.GetCount())
		NL();
}

void CppNodeWalker::WriteMethod(Method& m) {
	ZClass& cls = m.OwnerClass;
	
	for (int i = 0; i < m.Overloads.GetCount(); i++) {
		Overload& o = m.Overloads[i];
		
		if (!o.IsScanned)
			continue;
				
		bool dupe = false;
		for (int k = 0; k < i; k++) {
			Overload& o2 = m.Overloads[k];
			
			if (o.LogSig == o2.LogSig) {
				dupe = true;
				if (PrintDupeErrors)
					ErrorReporter::DupObject(cls.Name, o.NamePoint, o2.NamePoint, o.OwnerMethod.Name).PrettyPrint(Cout());
			}
		}
		
		if ((IgnoreDupes && !dupe) || !IgnoreDupes) {
			int written = 0;
			
			for (int k = 0; k < o.DepOver.GetCount(); k++)
				if (o.DepOver[k]->MDecWritten != CompilationUnitIndex) {
					WriteOverloadDeclaration(*o.DepOver[k]);
					o.DepOver[k]->MDecWritten = CompilationUnitIndex;
					written++;
				}
				
			if (written)
				NL();
			
			WriteOverload(o);
		}
	}
}

void CppNodeWalker::WriteOverload(Overload& overload) {
	WriteOverloadDefinition(overload);
	
	OpenOverload();
	WriteOverloadBody(overload, 1);
	CloseOverload();
}

void CppNodeWalker::WriteOverloadBody(Overload& overload, int indent) {
	ResetIndent(indent);
	
	for (int i = 0; i < overload.Nodes.GetCount(); i++)
		WalkStatement(overload.Nodes[i]);
}

}