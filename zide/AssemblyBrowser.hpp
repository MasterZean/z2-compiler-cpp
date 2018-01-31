#ifndef __ASSEMBLY_BROWSER_HPP__
#define __ASSEMBLY_BROWSER_HPP__

class DocEntry {
public:
	String Brief;
	WithDeepCopy<VectorMap<String, String>> Params;
	String Returns;
	String SeeAlso;
	String Code;
	String Key;
	WithDeepCopy<Index<String>> Links;
};

extern void FindDocLinks(Index<String>& links, const String& str);

class AssemblyBrowser: public ParentCtrl {
private:
	FrameTop<ParentCtrl> frame;
	Index<String> modules;
	bool showPaths = false;
	
	void OnSelectSource();
	int AddModule(int parent, const String& path, const String& ppath, const Image& img);
		
	void AddDocFile(const String& path);
	
public:
	typedef AssemblyBrowser CLASSNAME;
	
	TreeCtrl treModules;
	
	Callback WhenSelectSource;
	Callback1<const String&> WhenFileRemoved;
	Callback1<const String&> WhenFileSaved;
	Gate3<const Vector<String>&, const String&, const String&> WhenRenameFiles;
	
	ArrayMap<String, DocEntry> Docs;
	
	AssemblyBrowser();
	
	int AddModule(const String& aModule, int color);
	
	void ClearModules();
	
	String GetItem() {
		return treModules.GetNode(treModules.GetCursor()).key;
	}
		
	void SetShowPaths(bool show);
	
	void OnBar(Bar& bar);
	
	void OnAddFolder();
	void OnAddFile();
	void OnRenameFile();
	void OnRenameFolder();
	void OnRename();
	void OnDeleteFolder();
	void OnDeleteFile();
	void OnFileNameCopy();
	void OnDrag();
	void OnDrop(int ii, PasteClip& pc);
};

void GetAllChildren(const String& path, Vector<String>& sub);

#endif