#include "zide.h"

#include <z2clibex/Compiler.h>

extern ZPackage pak;

String TrimDocLinks(const String& str) {
	String s;
	
	int i = 0;
	int l = 0;
	
	while (i < str.GetCount()) {
		int first = str.Find('[', i);
		
		if (first != -1) {
			for (int j = l; j < first; j++)
				s << (char)str[j];
			l = first;
			
			int last = str.Find(']', first + 1);
			
			if (last != -1) {
				String t = str.Mid(first + 1, last - first - 1);
				
				int first2 = str.Find('[', last + 1);
				
				if (first2 == last + 1) {
					int last2 = str.Find(']', first2 + 1);
					
					if (last2 != -1) {
						s << "[C@B_ " << t << "]";

						i = last2;
						l = i + 1;
					}
				}
			}
		}
		else
			s << (char)str[i];
		
		i++;
	}
	
	return s;
}

String TrimDocLinksHtml(const String& str) {
	String s;
	
	int i = 0;
	int l = 0;
	
	while (i < str.GetCount()) {
		int first = str.Find('[', i);
		
		if (first != -1) {
			for (int j = l; j < first; j++)
				s << (char)str[j];
			l = first;
			
			int last = str.Find(']', first + 1);
			
			if (last != -1) {
				String t = str.Mid(first + 1, last - first - 1);
				
				int first2 = str.Find('[', last + 1);
				
				if (first2 == last + 1) {
					int last2 = str.Find(']', first2 + 1);
					
					if (last2 != -1) {
						String t2 = str.Mid(first2 + 1, last2 - first2 - 1);
						s << "<a href=\"" << t2 << ".html\">" << t << "</a>";

						i = last2;
						l = i + 1;
					}
				}
			}
		}
		else
			s << (char)str[i];
		
		i++;
	}
	
	return s;
}

void Zide::OnMenuHelpRebuildDocs() {
	Assembly ass;
	Compiler comp(ass);
	
	// add all packages to an assembly
	try {
		for (int i = 0; i < packages.GetCount(); i++)
			comp.AddPackage(packages[i], false);
	}
	catch (ZSyntaxError& exc) {
		StringStream ss;
		exc.PrettyPrint(nullptr, ss);
		
		splBottom.Show();
		
		String s = "Generating MD documentation...\n";
		s << ss.GetResult();
		console.Set(s.ToWString());
	
		console.ScrollEnd();
		console.ScrollLineUp();
		return;
	}
	
	header = LoadFile(docPath + "header");
	footer = LoadFile(docPath + "footer");
	
	String path = docPath + "..\\" + "api" + ".html";
	FileOut f(path);
	
	String headerMain = LoadFile(docPath + "..\\header");
	String footerMain = LoadFile(docPath + "..\\footer");
	
	f << headerMain;
	
	f << "<p>\n";
	f << "The Z2 compiler and build system are package based.\n";
	f << "</p>\n";
	
	f << "<p>\n";
	f << "Bellow you can find a list of the packages that make up the standard library.\n";
	f << "</p>\n";
	
	// traverse all packages
	for (int i = 0; i < packages.GetCount(); i++) {
		String& pak = packages[i];
		Vector<ZClass*> classes;
		
		Vector<String> sub;
		GetAllChildren(pak, sub);
		
		for (int j = 0; j < sub.GetCount(); j++) {
			String& path = sub[j];
			
			// is it a doc file
			if (path.EndsWith(".api.md")) {
				String fn = GetFileTitle(GetFileTitle(path));
				
				// look up the associated class in the assembly
				int ii = comp.LookUp.Find(fn);
				if (ii != -1) {
					ZSource* source = comp.FindSource(comp.LookUp[ii]);
					if (source) {
						for (int i = 0; i < source->ClassPrototypes.GetCount(); i++) {
							ZClass& cls = source->ClassPrototypes[i];
							
							if (cls.Scan.Namespace + cls.Scan.Name == fn) {
								classes.Add(&cls);
								
								String file = GetFileDirectory(comp.LookUp[ii]) + fn + ".api.md";
								String file2 = docPath + fn + ".html";
								
								FileOut f(file);
								FileOut f2(file2);
								
								OnGenerateDocTemp3(cls, f, f2);
							}
						}
					}
				}
			}
		}
		
		VectorMap<String, String> vv;
		
		if (classes.GetCount())
			f << "<h1>" << GetFileName(pak) << "</h1>\n";

		for (int j = 0; j < classes.GetCount(); j++) {
			ZClass& cls = *classes[j];
			
			String sss;
			sss << "<li><a href=\"pak/" << cls.Scan.Namespace << cls.Scan.Name << ".html\">" << cls.Scan.Name << " (<span class=\"muted\">" << cls.Scan.Namespace.Mid(0, cls.Scan.Namespace.GetCount() - 1) << ")</span><a></li>\n";
			vv.Add(cls.Scan.Name, sss);
		}
		
		SortByKey(vv);
		for (int j = 0; j < vv.GetCount(); j++) {
			f << vv[j];
		}
	}
	
	f << footerMain;
}

void Zide::OnAnnotation() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	if(annotation_popup.IsOpen())
		annotation_popup.Close();
	
	int ln = editor.GetActiveAnnotationLine();
	
	String ann = editor.GetAnnotation(ln);
	int aki = ann.Find(' ');
	
	if (aki == -1) {
		int ii = asbAss.Docs.Find(ann);
		if (ii == -1)
			return;
		
		DocEntry& doc = asbAss.Docs[ii];
		
		int lpi = ann.ReverseFind('.');
		String cls = ann.Mid(lpi + 1);
		String ns = ann.Mid(0, lpi + 1);
		String qtf = "[2 [C ";
		qtf << "[@B* class] " << DeQtf(ns) << "[@c " << DeQtf(cls) << "]&";
		qtf << "]&";
		
		String b = TrimDocLinks(doc.Brief);
		b.Replace("\n", "&");
		b = TrimBoth(b);
		if (b.EndsWith("&"))
			b = b.Mid(0, b.GetCount() - 1);
		qtf << b;
		
		annotation_popup.SetQTF(qtf);
		
		Rect r = editor.GetLineScreenRect(ln);
		int h = annotation_popup.GetHeight(580);
		h = min(h, 550);
		int y = r.top - h - 16;
		if(y < GetWorkArea().top)
			y = r.bottom;
		annotation_popup.SetRect(r.left, y, 600, h + 16);
	
		if (annotation_popup.IsOpen())
			return;
		annotation_popup.Ctrl::PopUp(&editor, false, false, true);
	
		return;
	}
	
	aki = ann.Find(' ', aki + 1);
	if (aki == -1)
		return;
	
	String key = ann.Mid(0, aki);
	int rki = ann.Find(' ', aki + 1);
	
	String rest = TrimBoth(ann.Mid(rki));
	String toParse = rest;
	rest.Replace(":", "[@B :]");
	rest.Replace(";", "[@B ;]");
	rest.Replace("<", "[@B <]");
	rest.Replace(">", "[@B >]");
	rest.Replace("ref ", "[C@B ref] ");
	rest.Replace("move ", "[C@B move] ");
	
	Index<String> parLook;
	
	int fci = toParse.Find("{");
	if (fci != -1 && toParse.EndsWith("}")) {
		toParse = toParse.Mid(fci + 1, toParse.GetCount() - 1 - fci);
		
		Vector<String> p = Split(toParse, ", ", false);
		
		for (int k = 0; k < p.GetCount(); k++) {
			int iii = p[k].Find(":");
			
			if (iii != -1) {
				String par = p[k].Mid(0, iii);
				if (par.StartsWith("ref "))
					par = par.Mid(4);
				else if (par.StartsWith("move "))
					par = par.Mid(5);
				parLook.Add(par);
			}
		}
	}
	
	fci = toParse.Find("(");
	if (fci != -1 && toParse.EndsWith(")")) {
		toParse = toParse.Mid(fci + 1, toParse.GetCount() - 1 - fci);
		
		Vector<String> p = Split(toParse, ", ", false);
		
		for (int k = 0; k < p.GetCount(); k++) {
			int iii = p[k].Find(":");
			
			if (iii != -1) {
				String par = p[k].Mid(0, iii);
				if (par.StartsWith("ref "))
					par = par.Mid(4);
				else if (par.StartsWith("move "))
					par = par.Mid(5);
				parLook.Add(par);
			}
		}
	}
	
	if (ln < 0 || ann.GetCount() == 0)
		return;
	
	int ii = asbAss.Docs.Find(key);
	if (ii == -1)
		return;
	
	DocEntry& doc = asbAss.Docs[ii];
	
	Vector<String> v = Split(ann, " ", false);
	String ns = v[0];
	int lpi = ns.ReverseFind('.');
	String cls = ns.Mid(lpi + 1);
	ns = ns.Mid(0, lpi + 1);
	String qtf = "[2 [C ";
	qtf << "[@B* class] " << DeQtf(ns) << "[@c " << DeQtf(cls) << "]&";
	if (v[2] == "const")
		qtf << "-|[@B* const] " << DeQtf(v[1]) << ";";
	else if (v[2] == "prop")
		qtf << "-|[@B* property] " << rest << ";";
	else if (v[2] == "def")
		qtf << "-|[@B* def] " << rest << ";";
	else if (v[2] == "defs")
		qtf << "-|[@B* static def] " << rest << ";";
	else if (v[2] == "func")
		qtf << "-|[@B* func] " << rest << ";";
	else if (v[2] == "funcs")
		qtf << "-|[@B* static func] " << rest << ";";
	else if (v[2] == "this")
		qtf << "-|[@B* this] " << rest << ";";
	else if (v[2] == "var")
		qtf << "-|[@B* var] " << DeQtf(v[1]) << ";";
	qtf << "]&&";
	String b = TrimDocLinks(doc.Brief);
	b.Replace("\n", "&");
	b = TrimBoth(b);
	if (b.EndsWith("&"))
		b = b.Mid(0, b.GetCount() - 1);
	qtf << b;
	
	int count = 0;
	for (int i = 0; i < doc.Params.GetCount(); i++) {
		if (parLook.Find(doc.Params.GetKey(i)) != -1) {
			count++;
			break;
		}
	}
	
	if (count || doc.Returns.GetCount())
		qtf << "&";
		
	if (count) {
		String qtf2;
		qtf2 << "&{{1:2 ";
		
		for (int i = 0; i < doc.Params.GetCount(); i++) {
			if (parLook.Find(doc.Params.GetKey(i)) != -1) {
				if (qtf2.GetCount() > 7)
					qtf2 << "||";
				qtf2 << "[C " << doc.Params.GetKey(i) << "]||" << doc.Params[i];
			}
		}
		
		qtf2 << "}}";
		
		qtf << qtf2;
	}
	
	if (doc.Returns.GetCount()) {
		qtf << "&{{1:2 ";
		qtf << "[@BC return]||" << doc.Returns;
		qtf << "}}";
	}
	
	qtf << "]";
	annotation_popup.SetQTF(qtf);
	
	Rect r = editor.GetLineScreenRect(ln);
	int h = annotation_popup.GetHeight(580);
	h = min(h, 550);
	int y = r.top - h - 16;
	if(y < GetWorkArea().top)
		y = r.bottom;
	annotation_popup.SetRect(r.left, y, 600, h + 16);

	if (annotation_popup.IsOpen())
		return;
	annotation_popup.Ctrl::PopUp(&editor, false, false, true);
}

void Zide::OnGenerateDocTemp() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	int yy = tabs.tabFiles.GetCursor();
	if (yy == -1)
		return;
	
	String file = tabs.tabFiles[yy].key.ToString();
	
	yy = explore.lstItems.GetCursor();
	if (yy == -1)
		return;

	ZItem zi = ValueTo<ZItem>(explore.lstItems.GetValue(yy));
	
	ZSource source;
	source.Package = &pak;
	pak.Path = lastPackage;
	source.Data = editor.Get();
	source.AddStdClassRefs();
		
	Scanner scanner(source, true);
	scanner.Scan();
	
	header = LoadFile(docPath + "header");
	footer = LoadFile(docPath + "footer");
	
	for (int i = 0; i < source.ClassPrototypes.GetCount(); i++) {
		ZClass& cls = source.ClassPrototypes[i];
		
		if (cls.Scan.Namespace + cls.Scan.Name == zi.Namespace) {
			file = GetFileDirectory(file) + zi.Namespace + ".api.md";
			String file2 = docPath + zi.Namespace + ".html";
			
			FileOut f(file);
			FileOut f2(file2);
			
			OnGenerateDocTemp3(cls, f, f2);
		}
	}
}

void Zide::OnGenerateDocTemp3(ZClass& cls, FileOut& f, FileOut& f2) {
	ArrayMap<String, DocEntry> docs;
	Index<String> links;
	
	f << "# ";
	if (cls.Scan.IsEnum)
		f << "enum ";
	else
		f << "class ";
	f << "*" << cls.Scan.Name << "*";
	String nss = cls.Scan.Namespace.Mid(0, cls.Scan.Namespace.GetCount() - 1);
	f << " from " << nss << "\n\n";
	
	int ii = asbAss.Docs.Find(cls.Scan.Namespace + cls.Scan.Name);
	if (ii != -1) {
		f << asbAss.Docs[ii].Brief << "\n";
	}
	
	f2 << header;
	
	f2 << "<ol class=\"breadcrumb\">\n";
	f2 << "<li class=\"breadcrumb-item\">" << nss << "</li>\n";
	f2 << "<li class=\"breadcrumb-item active\">"<< cls.Scan.Name << "</li>\n";
	f2 << "</ol>\n\n";

	f2 << "<h1>class " << cls.Scan.Name << "</h1>\n";
	f2 << "<pre><code class=\"cs\">namespace " << nss << "\n";
	f2 << "class "<< cls.Scan.Name << "\n";
	f2 << "</code></pre>\n";
	
	if (ii != -1) {
		String b = asbAss.Docs[ii].Brief;
		
		if (b.GetCount()) {
			FindDocLinks(links, b);
			b.Replace("\n", "<br/>\n");
			b.Replace("  ", "<br/>\n");
			
			f2 << "<div class=\"codesection\">\n";
			f2 << "<p>" << b << "</p>\n";
			f2 << "</div>\n";
		}
	}
	
	for (int i = 0; i < cls.Cons.GetCount(); i++) {
		Def& def = cls.Cons[i];
		
		if (def.Access != Entity::atPublic)
			continue;
		
		if (def.Name == "this") {
			int index = docs.FindAdd(def.Name);
			DocEntry& doc = docs[index];
			doc.Key = def.Name;
			if (doc.Brief.GetCount() == 0)
				doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
			
			for (int j = 0; j < def.Overloads.GetCount(); j++) {
				Overload& over = def.Overloads[j];
				
				if (over.IsNoDoc)
					continue;
				
				for (int k = 0; k < over.ParamPreview.GetCount(); k++)
					doc.Params.FindAdd(over.ParamPreview[k]);
				
				if (over.IsVoidReturn == false)
					doc.Returns = " ";
				
				if (!over.IsGenerated) {
					String s;
					s << def.Name;
					s << "{";
					CParser::Pos pos = over.CPosPar;
					const char *ch = pos.ptr;
					while ((*ch >= ' ') && (*ch != ')')) {
						s << *ch;
						ch++;
					}
					s << "}";
					if (doc.Code.GetCount())
						doc.Code << "\n";
					doc.Code << s << ";";
				}
			}
		}
	}
	
	for (int i = 0; i < cls.Cons.GetCount(); i++) {
		Def& def = cls.Cons[i];
		
		if (def.Access != Entity::atPublic)
			continue;
		
		if (def.Name != "this") {
			int index = docs.FindAdd(def.Name);
			DocEntry& doc = docs[index];
			doc.Key = def.Name;
			if (doc.Brief.GetCount() == 0)
				doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
			
			for (int j = 0; j < def.Overloads.GetCount(); j++) {
				Overload& over = def.Overloads[j];
				
				if (over.IsNoDoc)
					continue;
								
				for (int k = 0; k < over.ParamPreview.GetCount(); k++)
					doc.Params.FindAdd(over.ParamPreview[k]);
				
				if (over.IsVoidReturn == false)
					doc.Returns = " ";
				
				if (!over.IsGenerated) {
					String s = "this ";
					s << def.Name;
					s << "{";
					CParser::Pos pos = over.CPosPar;
					const char *ch = pos.ptr;
					while ((*ch >= ' ') && (*ch != ')')) {
						s << *ch;
						ch++;
					}
					s << "}";
					if (doc.Code.GetCount())
						doc.Code << "\n";
					doc.Code << s << ";";
				}
			}
		}
	}
	
	if (docs.GetCount()) {
		f << "## ";
		f << "Constructors\n\n";
		
		f2 << "<h2>Constructors</h2>\n";
		f2 << "<div class=\"codesection\">\n";
		
		for (int i = 0; i < docs.GetCount(); i++) {
			WriteDocEntry(f, f2, docs[i], links);
			if (i < docs.GetCount() - 1)
				f2 << "<hr/>\n";
		}
		
		f2 << "</div>";
		
		docs.Clear();
	}
	
	for (int i = 0; i < cls.Defs.GetCount(); i++) {
		Def& def = cls.Defs[i];
		
		int dc = 0;
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			Overload& over = def.Overloads[j];
			if (!over.IsNoDoc)
				dc++;
		}
		
		if (def.Access != Entity::atPublic || dc == 0)
			continue;
	
		int index = docs.FindAdd(def.Name);
		DocEntry& doc = docs[index];
		doc.Key = def.Name;
		if (doc.Brief.GetCount() == 0)
			doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
			
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			Overload& over = def.Overloads[j];
			
			if (over.IsNoDoc)
				continue;
			for (int k = 0; k < over.ParamPreview.GetCount(); k++)
				doc.Params.FindAdd(over.ParamPreview[k]);
			
			if (over.IsVoidReturn == false)
				doc.Returns = " ";
			
			if (!over.IsGenerated) {
				String s;
				if (over.IsStatic)
					s << "static ";
				if (over.IsConst)
					s << "func ";
				else
					s << "def ";
				s << def.Name;
				s << "(";
				CParser::Pos pos = over.CPosPar;
				const char *ch = pos.ptr;
				while ((*ch >= ' ') && (*ch != ')')) {
					s << *ch;
					ch++;
				}
				s << ")";
				ch++;
				while ((*ch >= ' ') && (*ch != '{' && *ch != ';' && *ch != '=')) {
					s << *ch;
					ch++;
				}
				if (doc.Code.GetCount())
					doc.Code << "\n";
				doc.Code << TrimBoth(s) + ";";
			}
		}
	}
	
	if (docs.GetCount()) {
		f << "## ";
		f << "Methods\n\n";

		f2 << "<h2>Methods</h2>\n";
		f2 << "<div class=\"codesection\">\n";
		
		for (int i = 0; i < docs.GetCount(); i++) {
			WriteDocEntry(f, f2, docs[i], links);
			if (i < docs.GetCount() - 1)
				f2 << "<hr/>\n";
		}
		
		f2 << "</div>";
		
		docs.Clear();
	}
		
	for (int i = 0; i < cls.Props.GetCount(); i++) {
		Def& def = cls.Props[i];

		if (def.Access != Entity::atPublic)
			continue;
				
		int index = docs.FindAdd(def.Name);
		DocEntry& doc = docs[index];
		doc.Key = def.Name;
		if (doc.Brief.GetCount() == 0)
			doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
		
		String s = "property ";
		s << def.Name;
		CParser::Pos pos = def.Pos[0];
		const char *ch = pos.ptr;
		while ((*ch >= ' ') && (*ch != '{') && *ch != '\n') {
			s << *ch;
			ch++;
		}
		
		bool get = false;
		bool set = false;
		
		for (int j = 0; j < def.Overloads.GetCount(); j++) {
			Overload& over = def.Overloads[j];
			
			if (over.IsGetter)
				get = true;
			else
				set = true;
		}
		s = TrimBoth(s);
		if (get && set) {
		}
		else if (get) {
			if (s.Find(" get = ") == -1) {
				if (!s.EndsWith(";"))
					s << ";";
				s <<" get;";
			}
		}
		else if (set) {
			if (!s.EndsWith(";"))
				s << ";";
			s <<" set;";
		}
		doc.Code = s;
	}
	
	if (docs.GetCount()) {
		f << "## ";
		f << "Properties\n\n";
		
		f2 << "<h2>Properties</h2>\n";
		f2 << "<div class=\"codesection\">\n";
		
		for (int i = 0; i < docs.GetCount(); i++) {
			WriteDocEntry(f, f2, docs[i], links);
			if (i < docs.GetCount() - 1)
				f2 << "<hr/>\n";
		}
		
		f2 << "</div>";
		
		docs.Clear();
	}
	
	for (int i = 0; i < cls.Constants.GetCount(); i++) {
		Constant& def = cls.Constants[i];
	
		if (def.Access != Entity::atPublic)
			continue;
		
		int index = docs.FindAdd(def.Name);
		DocEntry& doc = docs[index];
		doc.Key = def.Name;
		if (doc.Brief.GetCount() == 0)
			doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
		
		String s = "const ";
		s << def.Name;
		CParser::Pos pos = def.Skip;
		const char *ch = pos.ptr;
		while ((*ch >= ' ') && (*ch != '=') && *ch != ';' && *ch != ',') {
			s << *ch;
			ch++;
		}
		doc.Code = TrimBoth(s) + ";";
	}
	
	if (docs.GetCount()) {
		f << "## ";
		f << "Constants\n\n";
		
		f2 << "<h2>Constants</h2>\n";
		f2 << "<div class=\"codesection\">\n";
		
		for (int i = 0; i < docs.GetCount(); i++) {
			WriteDocEntry(f, f2, docs[i], links);
			if (i < docs.GetCount() - 1)
				f2 << "<hr/>\n";
		}
		
		f2 << "</div>";
		
		docs.Clear();
	}
	
	for (int i = 0; i < cls.Vars.GetCount(); i++) {
		Variable& def = cls.Vars[i];
	
		if (def.Access != Entity::atPublic)
			continue;
				
		int index = docs.FindAdd(def.Name);
		DocEntry& doc = docs[index];
		doc.Key = def.Name;
		if (doc.Brief.GetCount() == 0)
			doc.Brief << cls.Scan.Namespace << cls.Scan.Name << " " << def.Name;
		
		String s = "val ";
		s << def.Name;
		CParser::Pos pos = def.Skip;
		const char *ch = pos.ptr;
		while ((*ch >= ' ') && (*ch != '=') && *ch != ';') {
			s << *ch;
			ch++;
		}
		doc.Code = TrimBoth(s) + ";";
	}
	
	if (docs.GetCount()) {
		f << "## ";
		f << "Variables\n\n";
		
		f2 << "<h2>Variables</h2>\n";
		f2 << "<div class=\"codesection\">\n";
		
		for (int i = 0; i < docs.GetCount(); i++) {
			WriteDocEntry(f, f2, docs[i], links);
			if (i < docs.GetCount() - 1)
				f2 << "<hr/>\n";
		}
		
		f2 << "</div>";
		
		docs.Clear();
	}

	for (int j = 0; j < links.GetCount(); j++) {
		f << "[" << links[j] << "]: " << links[j] << ".api.md \"" << links[j] << "\"\n";
	}
	
	f2 << footer;
}

void Zide::WriteDocEntry(FileOut& f, FileOut& f2, DocEntry& doc, Index<String>& links) {
	f << "### " << doc.Key << "\n";
	if (doc.Code.GetCount()) {
		f << "\n" << "```C#" << "\n";
		f << doc.Code << "\n";
		f << "```" << "\n" << "\n";
	}
	
	f2 << "<h3>" << doc.Key << "</h3>\n";
	f2 << "<pre><code class=\"cs\">" << doc.Code << "</code></pre>\n";
	
	f << "#### Brief" << "\n";
	f2 << "<h4>" << "Brief" << "</h4>\n";
	
	int ii = asbAss.Docs.Find(doc.Brief);
	if (ii != -1) {
		DocEntry& exDoc = asbAss.Docs[ii];
		String b = exDoc.Brief;
		f << b;
		
		FindDocLinks(links, b);
		b.Replace("  ", "<br/>\n");
		f2 << "<p>\n" << TrimDocLinksHtml(b) << "\n";
		f2 << "</p>";
	}
	f << "\n";
	
	if (doc.Params.GetCount()) {
		f << "#### Parameters" << "\n";
		f2 << "<h4>Parameters</h4>" << "\n";
		f2 << "<blockquote>";
		for (int j = 0; j < doc.Params.GetCount(); j++) {
			if (ii == -1)
				f << "\n";
			f << "> *" << doc.Params.GetKey(j) << "* => ";
			if (ii != -1) {
				DocEntry& exDoc = asbAss.Docs[ii];
				int jj = exDoc.Params.Find(doc.Params.GetKey(j));
				if (jj != -1) {
					f << exDoc.Params[jj] << "  ";
					f2 << doc.Params.GetKey(j) << "=> " << exDoc.Params[jj] << "<br/>" << "\n";
				}
			}
			
			f << "\n";
		}
		f2 << "</blockquote>\n";
	}
	
	if (doc.Returns.GetCount()) {
		if (ii == -1)
			f << "\n";
		f << "#### Returns" << "\n";
		f2 << "<h4>Returns</h4>" << "\n";
		int ii = asbAss.Docs.Find(doc.Brief);
		if (ii != -1) {
			DocEntry& exDoc = asbAss.Docs[ii];
			String b = exDoc.Returns;
			f << "> " << b;
			
			f2 << "<blockquote>" << b << "</blockquote>" << "\n";
		}
		
		f << "\n";
	}
	
	int kk = asbAss.Docs.Find(doc.Brief);
	if (kk != -1) {
		if (ii == -1)
			f << "\n";
		DocEntry& exDoc = asbAss.Docs[kk];
		String b = exDoc.SeeAlso;
		if (b.GetCount() != 0) {
			f << "###### seealso " << b << "\n";
		}
	}
	
	f << "***" << "\n";
	f << "\n";
}

void Zide::LoadNavigation(ZSource& source) {
	int cur = explore.lstItems.GetCursor();
	
	explore.lstItems.Clear();
	explore.lstItems.Set(0, "aa", RawToValue(ZItem()));
	
	ZItem item;

	SmartEditor& editor = GetEditor();
	editor.words.Clear();
	editor.ClearAnnotations();
	
	for (int i = 0; i < source.ClassPrototypes.GetCount(); i++) {
		ZClass& cls = source.ClassPrototypes[i];
		
		int ii = asbAss.Docs.Find(cls.Scan.Namespace + cls.Scan.Name);
		if (ii != -1)
			editor.SetAnnotation(cls.Position.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), cls.Scan.Namespace + cls.Scan.Name);
			
		item.Kind = cls.Scan.IsEnum ? 2 : 1;
		item.Pos = cls.Position;
		String s = cls.Scan.Name;
		if (cls.Scan.Namespace.GetLength())
			s << " (" << cls.Scan.Namespace.Mid(0, cls.Scan.Namespace.GetLength() - 1) << ")";
		item.Name = s;
		item.Namespace = cls.Scan.Namespace + cls.Scan.Name;
		int node = explore.lstItems.Add(0, Image(), RawToValue(item));
						
		item.Kind = 0;
		
		for (int j = 0; j < cls.Constants.GetCount(); j++) {
			Constant& cst = cls.Constants[j];
			item.Kind = 10;
			item.Name = cst.Name;
			item.Pos = cst.Location;
			explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
			editor.words.Add(item);
			
			String key = cls.Scan.Namespace;
			key << cls.Scan.Name;
			key << " ";
			key << cst.Name;
			
			int ii = asbAss.Docs.Find(key);
			if (ii != -1)
				editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), key + " const");
		}
		
		for (int j = 0; j < cls.Vars.GetCount(); j++) {
			Variable& cst = cls.Vars[j];
			item.Kind = 11;
			item.Name = cst.Name;
			item.Pos = cst.Location;
			explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
			editor.words.Add(item);
			
			String key = cls.Scan.Namespace;
			key << cls.Scan.Name;
			key << " ";
			key << cst.Name;
					
			int ii = asbAss.Docs.Find(key);
			if (ii != -1)
				editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), key + " var");
		}
		
		for (int j = 0; j < cls.Cons.GetCount(); j++) {
			Def& def = cls.Cons[j];
			
			item.Kind = 5;
			item.Name = def.Name;
			if (def.Name != "this")
				editor.words.Add(item);
			for (int k = 0; k < def.Overloads.GetCount(); k++) {
				item.Kind = 7;
				Overload& over = def.Overloads[k];
				if (!over.IsGenerated) {
					s = "";
					if (over.IsCons == 2) {
						s << def.Name;
						item.Kind = 7;
					}
					s << "{";
					CParser::Pos pos = over.CPosPar;
					const char *ch = pos.ptr;
					while ((*ch >= ' ') && (*ch != ')')) {
						s << *ch;
						ch++;
					}
					s << "}";
					item.Name = s;
					item.Pos = over.Location;
					explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
					
					String key = cls.Scan.Namespace;
					key << cls.Scan.Name;
					key << " ";
					key << over.Name;
					
					int ii = asbAss.Docs.Find(key);
					if (ii != -1)
						editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), key + " this " + s);
				}
			}
		}
		
		for (int j = 0; j < cls.Props.GetCount(); j++) {
			Def& def = cls.Props[j];
			
			if (def.HasPGetter && !def.HasPSetter)
				item.Kind = 6;
			if (!def.HasPGetter && def.HasPSetter)
				item.Kind = 8;
			if (def.HasPGetter && def.HasPSetter)
				item.Kind = 9;
			item.Name = def.Name;
			editor.words.Add(item);
			
			s = "";
			s << def.Name;
			CParser::Pos pos = def.Pos[0];
			const char *ch = pos.ptr;
			while ((*ch >= ' ') && (*ch != '{') && *ch != '\n') {
				s << *ch;
				ch++;
			}
			
			item.Name = s;
			item.Pos = def.Location;
			explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
			
			String key = cls.Scan.Namespace;
			key << cls.Scan.Name;
			key << " ";
			key << def.Name;
			
			int ii = asbAss.Docs.Find(key);
			if (ii != -1)
				editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), key + " prop " + s);
		}
		
		for (int j = 0; j < cls.Defs.GetCount(); j++) {
			Def& def = cls.Defs[j];
			
			item.Kind = def.IsStatic ? 4 : 3;
			item.Name = def.Name;
			editor.words.Add(item);
			
			for (int k = 0; k < def.Overloads.GetCount(); k++) {
				Overload& over = def.Overloads[k];
				if (!over.IsGenerated) {
					s = "";
					s << def.Name;
					s << "(";
					CParser::Pos pos = over.CPosPar;
					const char *ch = pos.ptr;
					while ((*ch >= ' ') && (*ch != ')')) {
						s << *ch;
						ch++;
					}
					s << ")";
					ch++;
					while ((*ch >= ' ') && (*ch != '{' && *ch != ';' && *ch != '=')) {
						s << *ch;
						ch++;
					}
					item.Name = TrimBoth(s);
					item.Pos = over.Location;
					explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
					
					String key = cls.Scan.Namespace;
					key << cls.Scan.Name;
					key << " ";
					key << over.Name;
					
					int ii = asbAss.Docs.Find(key);
					if (ii != -1) {
						String v = key;
						if (over.IsConst)
							v << " func";
						else
							v << " def";
						if (over.Parent->IsStatic)
							v << "s";
						v << " " << s;
						editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), v);
					}
				}
			}
			
			if (def.Overloads.GetCount() == 0 && def.Template) {
				s = def.Name;
				s << "(";
				CParser::Pos pos = def.CPosPar;
				const char *ch = pos.ptr;
				while ((*ch >= ' ') && (*ch != ')')) {
					s << *ch;
					ch++;
				}
				s << ")";
				item.Name = s;
				item.Pos = def.Location;
				explore.lstItems.Add(node, Image(), item.Name, RawToValue(item));
				
				String key = cls.Scan.Namespace;
				key << cls.Scan.Name;
				key << " ";
				key << def.Name;
					
				int ii = asbAss.Docs.Find(key);
				if (ii != -1)
					editor.SetAnnotation(item.Pos.x - 1, TrimBoth(asbAss.Docs[ii].Brief).GetCount() ? ZImg::annot() : ZImg::annotm(), key + " def " + s);
			}
		}
		
		explore.lstItems.Open(node);
	}
	
	OpenFileInfo* info = GetInfo();
	if (!info)
		return;
	
	Index<String> classes;
	classes.Clear();
	
	for (int i = 0; i < source.References.GetCount(); i++) {
		String s = source.References[i];
		int j = s.ReverseFind('.');
		if (j != -1)
			s = s.Mid(j + 1);
		classes.Add(s);
	}
	
	for (int i = 0; i < source.References.GetCount(); i++) {
		item.Kind = 1;
		String s = source.References[i];
		int j = s.ReverseFind('.');
		if (j != -1)
			s = s.Mid(j + 1);
		item.Name = s;
		editor.words.Add(item);
	}
	
	if (classes != info->classes) {
		info->classes = pick(classes);
	}

	CSyntax::SetNames(CSyntax::HIGHLIGHT_Z2, info->classes);
	info->editor.Highlight("z2");
}

void Zide::LoadNavigation() {
	try {
		ZSource source;
		source.Data = GetEditor().Get();
		source.AddStdClassRefs();
		
		Scanner scanner(source, true);
		scanner.Scan();
		
		LoadNavigation(source);
	}
	catch (...) {
	}
}