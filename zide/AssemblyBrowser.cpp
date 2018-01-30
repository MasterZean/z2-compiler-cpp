#include "zide.h"

void GetAllChildren(const String& path, Vector<String>& sub) {
	FindFile ff;
	ff.Search(path + "\\*");

	while (ff) {
		String name = ff.GetName();
		if (ff.IsFolder()) {
			if (name != "." && name != "..")
				GetAllChildren(ff.GetPath(), sub);
		}
		else {
			if (name != "." && name != "..")
				sub.Add(ff.GetPath());
		}
		
		ff.Next();
	}
}

int PromptDeleteDontDeleteCancel(const char *qtf) {
	BeepQuestion();
	return Prompt(callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Delete"), t_("&Don't Delete"), t_("Cancel"), 0,
	              CtrlImg::remove(), NoButtonImage(), Null);
}

bool CanMoveFile(const String& path, const String& newPath) {
	FindFile ff(newPath);
	
#ifdef PLATFORM_WIN32
	return !ff || (ff && ToUpper(newPath) == ToUpper(path));
#else
	return !ff;
#endif
}

AssemblyBrowser::AssemblyBrowser() {
	AddFrame(frame.Height(2));
	Add(treModules);

	treModules.HSizePos().VSizePos();
	treModules.RenderMultiRoot().EmptyNodeIcon(CtrlImg::cross());
	treModules.NoRoot();
	treModules.NoWantFocus();
	treModules.Open(0);

	treModules.WhenSel = THISBACK(OnSelectSource);
	treModules.WhenBar = THISBACK(OnBar);
	treModules.WhenLeftDouble = THISBACK(OnRename);
}

void AssemblyBrowser::OnSelectSource() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String path = treModules[i];
	if (FileExists(path))
		WhenSelectSource();
}

void AssemblyBrowser::ClearModules() {
	treModules.Clear();
	modules.Clear();
}

void AssemblyBrowser::OnBar(Bar& bar) {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String path = treModules[i];
	if (DirectoryExists(path)) {
		bar.Add("Add file", CtrlImg::new_doc(), THISBACK(OnAddFile));
		bar.Add("Add folder", CtrlImg::MkDir(), THISBACK(OnAddFolder));
		
		bar.Separator();
		
		bar.Add("Rename", CtrlImg::write(), THISBACK(OnRenameFolder));
		bar.Add("Delete if empty", CtrlImg::remove(), THISBACK(OnDeleteFolder)).Enable(treModules.GetChildCount(i) == 0);
		
		bar.Separator();
		
		bar.Add("Copy folder path name", CtrlImg::copy(), THISBACK(OnFileNameCopy));
	}
	else {
		bar.Add("Rename", CtrlImg::write(), THISBACK(OnRenameFile));
		bar.Add("Delete", CtrlImg::remove(), THISBACK(OnDeleteFile));
		
		bar.Separator();
		
		bar.Add("Copy file path name", CtrlImg::copy(), THISBACK(OnFileNameCopy));
	}
}

void AssemblyBrowser::OnFileNameCopy() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String path = treModules[i];
	WriteClipboardText(NativePath(path));
}

void AssemblyBrowser::OnAddFolder() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
		
	String dname;
	if (EditText(dname, "New Folder", "New folder name")) {
		String path = treModules[i];
		path << "\\" << dname;
		path = NativePath(path);
		
		if (!DirectoryCreate(path)) {
			ErrorOK("[ph Could not create folder &-|[* " + DeQtf(path) + "]&]");
			return;
		}
		
		int count = treModules.GetChildCount(i);
		Vector<String> folders;
		
		for (int j = 0; j < count; j++) {
			int ch = treModules.GetChild(i, j);
			
			if (DirectoryExists((String)treModules[ch]))
				folders.Add(GetFileName((String)treModules[ch]));
		}
		
		for (int k = 0; k < folders.GetCount() - 1; k++) {
			String f1 = folders[k];
			String f2 = folders[k + 1];
			if (dname >= f1 && dname <= f2) {
				int newItem = treModules.Insert(i, k + 1, CtrlImg::Dir(), path, dname);
				treModules.SetCursor(newItem);
				return;
			}
		}
		
		int newItem = treModules.Insert(i, folders.GetCount(), CtrlImg::Dir(), path, dname);
		treModules.SetCursor(newItem);
	}
}

void AssemblyBrowser::OnAddFile() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
		
	String dname;
	if (EditText(dname, "New file", "New file name")) {
		String path = treModules[i];
		path << "\\" << dname;
		path = NativePath(path);
		if (FileExists(path)) {
			ErrorOK("[ph Could not create file&-|[* " + DeQtf(path) + "]&because it already exists!]");
			return;
		}
		
		FileOut f(path);
		f.Close();
		if (f.IsError()) {
			ErrorOK("[ph Could not open file&-|[* " + DeQtf(path) + "]&for writing!]");
			return;
		}
		
		int count = treModules.GetChildCount(i);
		Vector<String> files;
		
		for (int j = 0; j < count; j++) {
			int ch = treModules.GetChild(i, j);
			
			if (FileExists((String)treModules[ch]))
				files.Add(GetFileName((String)treModules[ch]));
			else
				files.Add(String::GetVoid());
		}
		
		for (int k = 0; k < files.GetCount() - 1; k++) {
			String f1 = files[k];
			String f2 = files[k + 1];
			if (!f1.IsVoid() && !f2.IsVoid() && dname >= f1 && dname <= f2) {
				int newItem = treModules.Insert(i, k + 1, ZImg::zsrc, path, dname);
				treModules.SetCursor(newItem);
				return;
			}
		}
		
		int newItem = treModules.Insert(i, files.GetCount(), ZImg::zsrc, path, dname);
		treModules.SetCursor(newItem);
	}
}

void AssemblyBrowser::OnRenameFile() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String dname = treModules.GetValue(i);
	String bak = dname;
	
	if (EditText(dname, "Rename file", "New file name") && dname != bak) {
		String path = treModules[i];
		String newPath = GetFileDirectory(path) + dname;
		if (!CanMoveFile(path, newPath)) {
			ErrorOK("[ph Could not rename file because&-|[* " + DeQtf(newPath) + "]&already exists!]");
			return;
		}
		
		WhenFileSaved(path);
		
		if (FileMove(path, newPath)) {
			TreeCtrl::Node n = treModules.GetNode(i);
			n.key = newPath;
			n.value = dname;
			treModules.SetNode(i, n);
		
			WhenFileRemoved(path);
			
			treModules.SetCursor(i);
			WhenSelectSource();
		}
	}
}

void AssemblyBrowser::OnRenameFolder() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String dname = treModules.GetValue(i);
	String bak = dname;
	if (EditText(dname, "Rename folder", "New folder name") && dname != bak) {
		String path = treModules[i];
		String oldPath = GetFileDirectory(path) + bak;
		String newPath = GetFileDirectory(path) + dname;
		
		Vector<String> sub;
		GetAllChildren(oldPath, sub);

		WhenRenameFiles(sub, oldPath, newPath);

		TreeCtrl::Node n = treModules.GetNode(i);
		n.key = newPath;
		n.value = dname;
		treModules.SetNode(i, n);
	}
}

void AssemblyBrowser::OnRename() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String dname = treModules.GetValue(i);
	String path = treModules[i];
	String newPath = GetFileDirectory(path) + dname;
	if (FileExists(newPath))
		OnRenameFile();
	else if (DirectoryExists(newPath))
		OnRenameFolder();
}

void AssemblyBrowser::OnDeleteFolder() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String path = treModules[i];
	int del = PromptDeleteDontDeleteCancel("[ph Are you sure you want to permanently delete the folder:&-|[* " + DeQtf(path) + "]&from disk?]");
	if (del == 1) {
		if (DirectoryDelete(path)) {
			treModules.Remove(i);
		}
	}
}

void AssemblyBrowser::OnDeleteFile() {
	int i = treModules.GetCursor();
	if (i == -1)
		return;
	
	String path = treModules[i];
	int del = PromptDeleteDontDeleteCancel("[ph Are you sure you want to permanently delete the file:&-|[* " + DeQtf(path) + "]&from disk?]");
	if (del == 1) {
		if (DeleteFile(path)) {
			treModules.Remove(i);
			WhenFileRemoved(path);
		}
	}
}

int AssemblyBrowser::AddModule(int parent, const String& path, const String& ppath, const Image& img) {
	int item = -1;
	if (path != ppath && path.Find(path) == 0)
		item = treModules.Add(parent, img, path, path.Mid(ppath.GetLength() + 1));
	else {
		String module = GetFileName(path);
		String modPath = GetFileDirectory(path);
		
		if (showPaths) {
			String v = module;
			v << " (" << modPath << ")";
			item = treModules.Add(parent, img, path, v);
		}
		else
			item = treModules.Add(parent, img, path, module);
	}

	FindFile ff;
	ff.Search(path + "/*");

	Vector<String> folders;
	Vector<String> files;
	
	while (ff) {
		if (ff.IsFolder()) {
			if (ff.GetName() != "." && ff.GetName() != "..")
				folders << ff.GetPath();
		}
		else
			files << ff.GetPath();
		
		ff.Next();
	}

	Sort(folders);
	Sort(files);
	
	for (int i = 0; i < folders.GetCount(); i++)
		AddModule(item, folders[i], path, CtrlImg::Dir());
	
	for (int i = 0; i < files.GetCount(); i++) {
		String name = GetFileName(files[i]);
		String ext = GetFileExt(name);

		if (ext == ".z2")
			treModules.Add(item, ZImg::zsrc, files[i], name);
		else if (name.EndsWith(".api.md"))
			AddDocFile(files[i]);
	}

	return item;
}

void FindDocLinks(Index<String>& links, const String& str) {
	int i = 0;
	
	while (i < str.GetCount()) {
		int first = str.Find('[', i);
		
		if (first != -1) {
			int last = str.Find(']', first + 1);
			
			if (last != -1) {
				int first2 = str.Find('[', last + 1);
				
				if (first2 == last + 1) {
					int last2 = str.Find(']', first2 + 1);
					
					if (last2 != -1) {
						links.FindAdd(str.Mid(first2 + 1, last2 - first2 - 1));
						i = last2;
					}
				}
			}
		}
		
		i++;
	}
}

void AssemblyBrowser::AddDocFile(const String& path) {
	String sub = GetFileTitle(GetFileTitle(path));
	
	FileIn f(path);
	
	DocEntry doc;
	
	String line, lastLine;
	String key;
	bool briefMode = false;
	bool paramMode = false;
	bool returnMode = false;
	bool seeAlso = false;
	bool lastEmpty = false;
	bool inBody = false;
	bool inClass = false;
	
	String param;
	
	while (!f.IsEof()) {
		lastLine = line;
		line = f.GetLine();

		if (inClass && line.StartsWith("#")) {
			FindDocLinks(doc.Links, doc.Brief);
			Docs.Add(sub, doc);
			
			inClass = false;
			doc.Brief = "";
		}
		
		if (line.StartsWith("### ")) {
			key = line.Mid(4);
		}
		else if (line == "***") {
			FindDocLinks(doc.Links, doc.Brief);

			if (inClass)
				Docs.Add(sub, doc);
			else
				Docs.Add(sub + " " + key, doc);
			
			// reset
			briefMode = false;
			paramMode = false;
			returnMode = false;
			lastEmpty = false;
			seeAlso = false;
			inClass = false;
			inBody = false;
			
			doc.Brief = "";
			doc.Params.Clear();
			doc.Returns = "";
			doc.SeeAlso = "";
		}
		else if (line == "#### Brief") {
			briefMode = true;
			inBody = true;
		}
		else if (line.StartsWith("# ")) {
			inClass = true;
		}
		else if (line == "" && (inBody || inClass)) {
			lastEmpty = true;
		}
		else if (line == "#### Parameters") {
			briefMode = false;
			paramMode = true;
			returnMode = false;
			lastEmpty = false;
			seeAlso = false;
			inClass = false;
		}
		else if (paramMode && line.StartsWith("> *")) {
			param = line.Mid(3);
			int iii = param.Find("* => ");
			if (iii != -1) {
				param = param.Mid(0, iii);
				line = TrimBoth(line.Mid(iii + 5 + 3));
				doc.Params.Add(param, line);
			}
		}
		else if (line == "#### Returns") {
			briefMode = false;
			paramMode = false;
			returnMode = true;
			lastEmpty = false;
			seeAlso = false;
			inClass = false;
		}
		else if (line.StartsWith("###### seealso ")) {
			briefMode = false;
			paramMode = false;
			returnMode = false;
			lastEmpty = false;
			seeAlso = true;
			inClass = false;
			doc.SeeAlso = line.Mid(15);
		}
		else if (inBody) {
			if (briefMode) {
				if (lastEmpty) {
					doc.Brief << "\n";
					lastEmpty = false;
				}
				doc.Brief << line << "\n";
			}
			if (paramMode) {
				doc.Params.Add(param, line);
			}
			if (returnMode) {
				doc.Returns = line;
				if (doc.Returns.StartsWith("> "))
					doc.Returns = line.Mid(2);
			}
		}
		else if (inClass) {
			if (lastEmpty) {
				if (doc.Brief.GetCount())
					doc.Brief << "\n";
				lastEmpty = false;
			}
			doc.Brief << line << "\n";
		}
	}
}

int AssemblyBrowser::AddModule(const String& aModule, int color) {
	int i = modules.Find(aModule);
	
	if (i == -1) {
		modules.Add(aModule);
		if (color == 0)
			return AddModule(0, aModule, aModule, CtrlImg::Dir());
		else if (color == 1)
			return AddModule(0, aModule, aModule, ZImg::libDirUsed());
		else if (color == 2)
			return AddModule(0, aModule, aModule, ZImg::libDirNotUsed());
	}
	
	return i;
}

void AssemblyBrowser::SetShowPaths(bool show) {
	showPaths = show;
	
	for (int i = 0; i < treModules.GetChildCount(0); i++) {
		int j = treModules.GetChild(0, i);
		String path = treModules.GetNode(j).key;
		String module = GetFileName(path);
		String modPath = GetFileDirectory(path);
		
		if (showPaths) {
			String v = module;
			v << " (" << modPath << ")";
			treModules.Set(j, path, v);
		}
		else
			treModules.Set(j, path, module);
	}
}