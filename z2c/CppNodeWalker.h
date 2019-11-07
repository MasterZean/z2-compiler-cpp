#ifndef _z2c2_CppNodeWalker_h_
#define _z2c2_CppNodeWalker_h_

#include <Core/Core.h>

#include "Node.h"
#include "Assembly.h"
#include "IRGenerator.h"

namespace Z2 {
	
using namespace Upp;

class CppNodeWalker {
public:
	CppNodeWalker(Assembly& aAss, Stream& ss): ass(aAss), stream(ss) {
	}
	
	void Walk(Node* node);
	
	void WalkStatement(Node* node) {
		if (node->NT == NodeType::Block)
			Walk(node);
		else if (node->NT != NodeType::Goto) {
			SS();
			Walk(node);
			
			if (node->NT != NodeType::If && node->NT != NodeType::While)
				stream << ";";
		
			if (DebugOriginalLine && node->OriginalLine)
				stream << "\t\t\t\t // " << node->OriginalLine;
			
			if (node->NT != NodeType::If && node->NT != NodeType::While)
				NL();
			else
				stream << " ";
		}
	}
	
	void WalkNode(ConstNode& node);
	void WalkNode(VarNode& node);
	void WalkNode(MemNode& node);
	void WalkNode(BlockNode& node);
	void WalkNode(OpNode& node);
	void WalkNode(UnaryOpNode& node);
	void WalkNode(CastNode& node);
	void WalkNode(CallNode& node);
	void WalkNode(RetNode& node);
	void WalkNode(AssignNode& node);
	void WalkNode(ListNode& node);
	void WalkNode(IfNode& node);
	void WalkNode(WhileNode& node);
	void WalkNode(GotoNode& node);
	
	void SS() {
		for (int i = 0; i < indent; i++)
			stream << "\t";
	}
	
	void NL() {
		stream << "\r\n";
		line++;
	}
	
	void ResetIndent(int ind = 0) {
		indent = ind;
	}
	
	bool CPP = true;
	bool DebugOriginalLine = false;
	
	void WriteClassName(const ZClass& ce) {
		stream << ce.BackendName;
	}
	
	void WriteClass(ZClass& cls);
	
	void WriteClassVars(ZClass& cls);
	void WriteVar(Variable& var);
	void WriteVarValue(Variable& var);
	
	void WriteOverloadDefinition(Overload &over);
	void WriteOverloadDeclaration(Overload &over);
	
	void OpenOverload() {
		stream << " {";
		NL();
	}
	
	void CloseOverload() {
		stream << "}";
		NL();
		NL();
	}
	
	bool WriteReturnType(Overload &over);
	void WriteOverloadNameParams(Overload &over);
	void WriteParams(Overload &over);
	void WriteAssemblyParams(Stream& s, Overload &over);
	void WriteMangledParams(Overload &over);
	
	void WriteMethod(Method& m);
	void WriteOverloadBody(Overload& overload, int indent = 0);
	void WriteOverload(Overload& overload);
	
	bool CommentZMethod = false;
	bool CommentCMangled = false;
	bool IgnoreDupes = true;
	int  CompilationUnitIndex = 0;
	bool PrintDupeErrors = true;
	
private:
	Assembly& ass;
	Stream& stream;
	
	int indent = 0;
	int line = 1;
};

}

#endif
