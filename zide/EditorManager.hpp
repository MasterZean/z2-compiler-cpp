#ifndef __EDITOR_MANAGER__
#define __EDITOR_MANAGER__

#include "ItemDisplay.hpp"

class Settings {
public:
	bool   ShowTabs = false;
	bool   ShowSpaces = false;
	bool   ShowNewlines = false;
	bool   WarnSpaces = true;
	int    TabSize = 4;
	bool   IndentSpaces = false;
	bool   ShowLineNums = true;
	bool   HighlightLine = true;
	int    TabPos = AlignedFrame::TOP;
	int    TabClose = AlignedFrame::RIGHT;
	int    LinePos = 96;
	Color  LineColor = LtGray();
	int    ScopeHighlight = 2;
	int    Brackets = 1;
	bool   Thousands = true;
	
	String Style;
	
	void Serialize(Stream& s) {
		s % ShowTabs % ShowSpaces % ShowNewlines % WarnSpaces % TabSize % IndentSpaces
		  % ShowLineNums % HighlightLine % TabPos % TabClose % LinePos % LineColor % Style
		  % ScopeHighlight % Brackets % Thousands;
	}
};

class SmartEditor: public CodeEditor {
public:
	typedef SmartEditor CLASSNAME;
	
	ColumnList popUp;
	Vector<ZItem>  words;
	bool ignoreFocus = false;
	bool exactMatch = false;
	
	SmartEditor() {
		popUp.SetDisplay(Single<ItemDisplay>());
		popUp.WhenLeftClick = THISBACK(OnAutoSelect);
	}
	
	void Update(const String& w);
	void DoPopup(Point p);
	
	void OnAutoSelect();
	
	virtual bool Key(dword key, int count);
	virtual void LostFocus();
	virtual void LeftDown(Point p, dword flags);
	
	Callback1<SmartEditor*> WhenAssistChange;

private:
	void Feed(const String& wrd, int ofss);
};

class OpenFileInfo {
public:
	SmartEditor editor;
	Index<String> classes;
	bool IsChanged;
	uint64 Hash;

	OpenFileInfo(): IsChanged(false), Hash(0) {
	}
};

class EditorManager: public ParentCtrl {
public:
	typedef EditorManager CLASSNAME;

	ParentCtrl edtCode;
	FileTabs tabFiles;
	ArrayMap<WString, OpenFileInfo> files;

	Callback WhenEditorCursor;
	Callback WhenEditorChange;
	Callback WhenTabChange;
	Callback WhenAnnotation;

	EditorManager();

	void Open(const String& item, bool forceReload = false);
	void Save(int i);
	void SaveAll();
	void SaveAllIfNeeded();
	void RemoveFile(const String& item);

	bool IsChanged(int i);
	void SetChanged(int i, bool changed);

	int GetCount() const {
		return tabFiles.GetCount();
	}

	int GetCursor() const {
		return tabFiles.GetCursor();
	}

	bool PromptSaves();
	
	static void SetSettings(CodeEditor& editor, Settings& settings, const String& syntax);
	
	void SetSettings(Settings& settings);

private:
	Settings settings;
	
	OpenFileInfo& GetInfo(int i);

	void OnTabChange();
	void OnEditorChange();
	void OnEditorCursor();
	bool OnCancelClose(Value val);
	bool OnCancelCloseSome(ValueArray arr);
	void OnTabClose(Value val);
	void OnTabCloseSome(ValueArray arr);
	
	void OnAssistChange(SmartEditor* e);
	
	void OnAnnotation();
};

#endif