#include "CppNodeWalker.h"

void BaseCppNodeWalker::Walk(ConstNode& node, Stream& stream) {
	if (node.Tt.Class == ass.CQWord) {
		if (IsNull(node.IntVal))
			stream << "9223372036854775808ull";
		else
			stream << Format64((uint64)node.IntVal) << "ull";
	}
	else if (node.Tt.Class == ass.CLong) {
		if (IsNull(node.IntVal))
			stream << "-9223372036854775808ll";
		else
			stream << IntStr64(node.IntVal) << "ll";
	}
	else if (node.Tt.Class == ass.CDWord || node.Tt.Class == ass.CWord || node.Tt.Class == ass.CByte) {
		if (node.Base == 16)
			stream << "0x" << Format64Hex((uint32)node.IntVal) << 'u';
		else
			stream << Format64((uint32)node.IntVal) << 'u';
	}
	else if (node.Tt.Class == ass.CPtrSize) {
		if (node.Base == 16)
			stream << "0x" << Format64Hex(node.IntVal) << "u";
		else
			stream << Format64(node.IntVal) << "u";
	}
	else if (node.Tt.Class == ass.CInt || node.Tt.Class == ass.CSmall || node.Tt.Class == ass.CShort) {
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
	else if (node.Tt.Class == ass.CFloat) {
		if (node.DblVal != node.DblVal) {
			stream << "((float)(1e+300 * 1e+300) * 0.0f)";
		}
		else if (node.DblVal == (float)(1e+300 * 1e+300))
			stream << "((float)(1e+300 * 1e+300))";
		else if (node.DblVal == -(float)(1e+300 * 1e+300))
			stream << "(-(float)(1e+300 * 1e+300))";
		else {
			if (IsNull(node.DblVal)) {
				stream << "-1.79769e+308" << "\f";
			}
			else {
				String s = FormatDouble(node.DblVal);
				stream << s;
				if (s.Find('.') == -1 && s.Find('e') == -1)
				    stream << ".0";
				stream << 'f';
			}
		}
	}
	else if (node.Tt.Class == ass.CDouble) {
		if (node.DblVal != node.DblVal) {
			stream << "((float)(1e+300 * 1e+300) * 0.0f)";
		}
		else if (node.DblVal == (float)(1e+300 * 1e+300))
			stream << "((float)(1e+300 * 1e+300))";
		else if (node.DblVal == -(float)(1e+300 * 1e+300))
			stream << "(-(float)(1e+300 * 1e+300))";
		else {
			if (IsNull(node.DblVal)) {
				stream << "-1.79769e+308";
			}
			else {
				String s = FormatDouble(node.DblVal);
				stream << s;
				if (s.Find('.') == -1 && s.Find('e') == -1)
					stream << ".0";
			}
		}
	}
	else if (node.Tt.Class == ass.CBool) {
		if (node.IntVal == 0)
			stream << "false";
		else
			stream << "true";
	}
	else if (node.Tt.Class == ass.CChar) {
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
	else if (node.Tt.Class == ass.CCls) {
		ZClass& ccc = ass.Classes[(int)node.IntVal];
		stream << "::Class(" << ccc.RTTIIndex << ")";
	}
	else if (node.Tt.Class == ass.CNull)
		stream << "nullptr";
	else if (node.Tt.Class == ass.CString)
		stream << "S_[" << (int)node.IntVal << ']';
	else if (node.Tt.Class->Scan.IsEnum) {
		ZClass& ec = *node.Tt.Class;
		
		for (int i = 0; i < ec.Constants.GetCount(); i++)
			if (node.IntVal == i) {
				stream << ec.BackendName << "::" << ec.Constants[i].Name;
				return;
			}
			
		stream << IntStr64(node.IntVal);
	}
	else
		ASSERT_(0, "Invalid const node");
}

void BaseCppNodeWalker::WriteLocalCArrayLiteralElement(Stream& cs, const ZClass& ce, Node& n, const String& name, int index) {
	String indexStr;
	if (index == -1)
		indexStr = "_forIndex";
	else
		indexStr = IntStr(index);
	
	if (ce.CoreSimple) {
		cs << name << "[" << indexStr << "] = ";
		WalkNode(&n);
		
		return;
	}
	
	if ((n.NT == NodeType::Temporary && ((TempNode&)n).Overload->IsCons == 2)) {
		TempNode& t = (TempNode&)n;
		cs << name << "[" << indexStr << "].";
		cs << t.Overload->BackendName;
		
		cs << '(';
		for (int i = 0; i < t.Params.GetCount(); i++) {
			Node* pp = t.Params[i];
			WalkNode(pp);
			if (i < t.Params.GetCount() - 1)
				cs << ", ";
		}
		cs << ')';
	}
	else {
		cs << "new (&" << name << "[" << indexStr << "]) ";
		WriteClassName(ce);
		cs << "(";
		WalkNode(&n);
		cs << ")";
	}
}

void BaseCppNodeWalker::WriteLocalCArrayLiteral(Stream& cs, const ZClass& ce, const RawArrayNode& list, const String& name, int count) {
	String countStr = IntStr(count);
	
	int temprawSize = rawSize;
	int temprawIndex = rawIndex;
	rawSize = list.Array.GetCount();
	
	if (ce.CoreSimple) {
		WriteClassName(ce);
		cs << ' ' << name;
		cs << "[" << countStr << ']';
	}
	else {
		/*cs << "alignas(";
		WriteClassName(ce);
		cs << ") ";*/
		
		/*cs << "__declspec(align(sizeof(";
		WriteClassName(ce);
		cs << ")) ";*/
		
		/*cs << "__declspec(align(4";
		//WriteClassName(ce);
		cs << ")) ";*/
		
		cs << "char _" << name;
		cs << "[" << countStr << " * sizeof(";
		WriteClassName(ce);
		cs << ")]";
		ES();
		
		NL();
		WriteClassName(ce);
		cs << "* " << name << " = (";
		WriteClassName(ce);
		cs << "*)_" << name;
	
	}
	ES();
	
	for (int i = 0; i < list.Ellipsis; i++) {
		NL();
		WriteLocalCArrayLiteralElement(cs, ce, *list.Array[i], name, i);
		ES();
	}
	
	NL();
	cs << "for (int _forIndex = " << list.Ellipsis << "; _forIndex < " << countStr << "; _forIndex++)";
	EL();
	
	indent++;
	
	NL();
	WriteLocalCArrayLiteralElement(cs, ce, *list.Array[list.Ellipsis], name);
	indent--;
	
	if (!ce.CoreSimple) {
		ES();
		
		NL();
		cs << name << " = (";
		WriteClassName(ce);
		cs << "*)_" << name;
		ES();
		
		NL();
		cs << "ZArrayDestroyer<";
		WriteClassName(ce);
		cs << "> __" << name << "(" << name << ", " << countStr << ")";
	}
	
	rawSize = temprawSize;
	rawIndex = temprawIndex;
}

bool BaseCppNodeWalker::WriteReturnType(Stream& cs, Overload &over) {
	ZClass& cls = over.Class();
	
	// constructors
	if (over.IsCons == 1) {
		if (cls.CoreSimple) {
			if (cls.MIsRawVec)
				cs << "void";
			else
				cs << cls.BackendName;
			
			return true;
		}
		
		return false;
	}
	else if (over.IsCons == 2) {
		cs << cls.BackendName;
		
		if (cls.CoreSimple == false)
			cs << "&";
		
		return true;
	}

	// normal functions
	if (over.IsClassCopyOperator())
		cs << cls.BackendName << "&";
	else if (over.IsClassMoveOperator())
		cs << cls.BackendName << "&";
	else if (!over.IsDest) {
		if (ass.IsPtr(over.Return.Tt))
			cs << over.Return.Tt.Next->Class->BackendName << "*";
		else if (over.Return.IsRef)
			cs << over.Return.Tt.Class->BackendName << "*";
		else {
			cs << over.Return.Tt.Class->BackendName;
			if (over.Return.Tt.Class->Scan.IsEnum)
				cs << "::Type";
		}
	}
	
	return true;
}

void BaseCppNodeWalker::WriteOverloadDefinition(Stream& cs, Overload &over) {
	ZClass& cls = over.Class();
	
	if (over.IsVirtual)
		cs << "virtual ";
	else if (over.IsInline)
		cs << "inline ";
	// TODO: fix
	else if (over.IsClassCopyCon() || over.IsClassMoveCon() || over.IsClassCopyOperator() || over.IsClassMoveOperator() || over.IsClassEqOperator() || over.IsClassNeqOperator())
		cs << "inline ";
	
	if (cls.CoreSimple || over.IsStatic)
		cs << "static ";
	
	if (WriteReturnType(cs, over))
		cs << " ";
	
	WriteOverloadNameParams(cs, over);
}

void BaseCppNodeWalker::WriteOverloadDeclaration(Stream& cs, Overload &over) {
	ZClass& cls = over.Class();
	
	// TODO: fix for BindName
	if (over.BindName.GetCount())
		cs << "extern \"C\" ";
	
	// write return type
	if (WriteReturnType(cs, over))
		cs << " ";
	
	// write call convention
	if (over.IsStdCall)
		 cs << "__stdcall ";
	if (over.IsCDecl)
		 cs << "__CDECL ";
	
	// write class name
	if (cls.CoreSimple) {
		if (cls.FromTemplate)
			cs << cls.BackendName;
		else
			cs << cls.Scan.Name;
	}
	else if (over.BindName.GetCount() == 0)
		cs << cls.BackendName;
	
	// TODO: fix for BindName
	if (over.BindName.GetCount() == 0)
		cs << "::";
	
	WriteOverloadNameParams(cs, over);
}

void BaseCppNodeWalker::WriteOverloadVoidingList(Stream& cs, Overload &over) {
	ZClass& cls = over.Class();
	
	// write initializers
	if (over.IsCons == 1 && cls.Vars.GetCount()) {
		bool first = true;
		
		for (int i = 0; i < cls.Vars.GetCount(); i++) {
			Variable& c = cls.Vars[i];
			
			if (!c.IsStatic && !c.I.Tt.Class->CoreSimple) {
				if (first)
					cs << ": ";
				else
					cs << ", ";
				
				cs << c.Name << "(_Void)";
				
				first = false;
			}
		}
	}
}

void BaseCppNodeWalker::WriteOverloadNameParams(Stream& cs, Overload &over) {
	ZClass& cls = over.Class();
	
	// name and parameters
	// special functions
	if (over.IsClassCopyCon()) {
		cs << cls.BackendName << "(const " << cls.BackendName << "& _copy)";
		return;
	}
	else if (over.IsClassMoveCon()) {
		cs << cls.BackendName << "(" << cls.BackendName << "&& _copy)";
		return;
	}
	else if (over.IsClassCopyOperator()) {
		cs << "operator=" << "(const " << cls.BackendName << "& _copy)";
		return;
	}
	else if (over.IsClassMoveOperator()) {
		cs << "operator=" << "(" << cls.BackendName << "&& _copy)";
		return;
	}
	else if (EqOperator && over.IsClassEqOperator()) {
		cs << "operator==" << "(const " << cls.BackendName << "& _copy)";
		if (WriteCtQual && over.IsConst)
			cs << " const ";
		
		return;
	}
	else if (EqOperator && over.IsClassNeqOperator()) {
		cs << "operator!=" << "(const " << cls.BackendName << "& _copy)";
		if (WriteCtQual && over.IsConst)
			cs << " const ";
		
		return;
	}
	
	// write parameters
	// normal functions
	if (over.IsCons == 1) {
		if (cls.CoreSimple)
			cs << over.BackendName;
		else
			cs << cls.BackendName;
		cs << "(";
		WriteParams(cs, over, cls, false);
	}
	else if (over.IsCons == 2) {
		cs << over.BackendName;
		cs << "(";
		WriteParams(cs, over, cls, false);
	}
	else if (cls.CoreSimple) {
		if (cls.Scan.Name != over.Name)
			cs << over.BackendName;
		cs << "(";
		WriteParams(cs, over, cls, true);
		
	}
	else if (over.BindName.GetCount()) {
		cs << over.BindName << "(";
		WriteParams(cs, over, cls, false);
	}
	else {
		cs << over.BackendName << '(';
		WriteParams(cs, over, cls, false);
	}
}

void BaseCppNodeWalker::WriteParams(Stream& cs, Overload& over, ZClass& cls, bool ths) {
	if (ths) {
		if (cls.MIsRawVec) {
			if (over.IsStatic == false) {
				cs << cls.T->BackendName << "*";
				cs << " " << "_this";
				cs << ", size_t _" << "this" << "_len";
				
				if (over.Params.GetCount())
					cs << ", ";
			}
		}
		else {
			if (ass.IsPtr(cls.Tt))
				cs << "void" << '*';
			else {
				cs << cls.BackendName;
				if (cls.Scan.IsEnum)
					cs << "::Type";
				
				if (!cls.CoreSimple)
					cs << '&';
				cs << ' ';
			}
			cs << "_this";
			if (over.Params.GetCount())
				cs << ", ";
		}
	}
	else if (cls.MIsRawVec) {
		cs << cls.T->BackendName << "* _this, size_t _this_len";
		if (over.Params.GetCount())
			cs << ", ";
	}
	
	for (int j = 0; j < over.Params.GetCount(); j++) {
		Variable& p = over.Params[j];

		if (ass.IsPtr(p.I.Tt)) {
			if (p.I.Tt.Next->Class->MIsRawVec)
				cs << p.I.Tt.Class->T->BackendName << "*";
			else
				cs << p.I.Tt.Next->Class->BackendName << "*";
			cs << " " << p.Name;
		}
		else if (p.I.IsRef) {
			if (p.I.Tt.Class->MIsRawVec) {
				cs << p.I.Tt.Class->T->BackendName << "*";
				cs << " " << p.Name;
				if (p.I.Tt.Param == -1)
					cs << ", size_t _" << p.Name << "_len";
			}
			// TODO: ADD OPT
			else if (p.I.Tt.Class->FromTemplate && p.I.Tt.Class->TBase == ass.CSlice) {
				cs << p.I.Tt.Class->T->BackendName << "*";
				cs << " " << p.Name;
				//if (p.I.Tt.Param == -1)
				cs << ", size_t *_" << p.Name << "_len";
			}
			else {
				cs << p.I.Tt.Class->BackendName << "*";
				cs << " " << p.Name;
			}
		}
		else if (p.I.Tt.Class->MIsRawVec) {
			cs << p.I.Tt.Next->Class->BackendName << "*";
			cs << " " << p.Name;
			if (p.I.Tt.Param == -1)
				cs << ", size_t _" << p.Name << "_len";
		}
		else if (p.I.Tt.Class->FromTemplate && p.I.Tt.Class->TBase == ass.CSlice) {
			cs << p.I.Tt.Class->T->BackendName << "*";
			cs << " " << p.Name;
			//if (p.I.Tt.Param == -1)
			cs << ", size_t *_" << p.Name << "_len";
		}
		else if (p.IsCppRef) {
			if (WriteCtQual && p.I.IsConst)
				cs << "const ";
			cs << p.I.Tt.Class->BackendName;
			if (!p.I.Tt.Class->CoreSimple)
				cs << "&";
			cs << " " << p.Name;
		}
		else {
			cs << p.I.Tt.Class->BackendName;
			if (p.I.Tt.Class->Scan.IsEnum)
				cs << "::Type";
			cs << " " << p.Name;
		}

		if (j < over.Params.GetCount() - 1)
			cs << ", ";
	}

	cs << ")";
	
	if (WriteCtQual && over.IsConst && !cls.CoreSimple && !over.IsStatic)
		cs << " const";
}

