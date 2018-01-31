#ifndef _zide_zide_h
#define _zide_zide_h

#include <CtrlLib/CtrlLib.h>
#include <CodeEditorFork/CodeEditor.h>
#include <TabBar/TabBar.h>

using namespace Upp;

#include "ItemDisplay.hpp"

#include "AssemblyBrowser.hpp"
#include "EditorManager.hpp"

#define LAYOUTFILE <zide/zide.lay>
#include <CtrlCore/lay.h>

#include <z2clib/BuildMethod.h>
#include <z2clib/StopWatch.h>

class Console: public LineEdit {
public:
	bool verbosebuild = false;

	Console() {
		SetReadOnly();
	}

	Callback WhenLeft;

	virtual void LeftUp(Point p, dword flags) {
		LineEdit::LeftUp(p, flags);
		WhenLeft();
	}
	
	void ScrollLineUp() {
		sb.LineUp();
	}
	
	Console& operator<<(const String& s)      { Append(s); return *this; }
	
	void Append(const String& s) {
		Insert(total, s);
	}
};

class BuildMethodsWindow: public WithBuildMethodsLayout<TopWindow> {
public:
	typedef BuildMethodsWindow CLASSNAME;
	
	BuildMethodsWindow(Vector<BuildMethod>& meth): methods(meth) {
		CtrlLayout(*this, "Build methods");
		Sizeable();
		
		lstMethods.NoRoundSize();
		lstMethods.WhenAction = THISBACK(OnBuildMethodSelect);
		
		for (int i = 0; i < methods.GetCount(); i++) {
			BuildMethod& bm = methods[i];
			lstMethods.Add(bm.Name);
		}
				
		lstLib32.SetReadOnly();
		lstLib64.SetReadOnly();
		
		btnOk.Ok();
		Acceptor(btnOk, IDOK);
	}
	
	void OnBuildMethodSelect() {
		int index = lstMethods.GetCursor();
		if (index == -1)
			return;
		
		BuildMethod& bm = methods[index];
		
		edtName.SetText(bm.Name);
		
		if (bm.Type == BuildMethod::btGCC) {
			optGCC.Set(1);
			optMSC.Set(0);
		}
		else if (bm.Type == BuildMethod::btMSC) {
			optGCC.Set(0);
			optMSC.Set(1);
		}
		else {
			optGCC.Set(0);
			optMSC.Set(0);
		}
		
		edtCompiler.SetText(bm.Compiler);
		edtSdk.SetText(bm.Sdk);

		lstLib32.Clear();
		for (int i = 0; i < bm.Lib32.GetCount(); i++)
			lstLib32.Add(bm.Lib32[i]);
		
		lstLib64.Clear();
		for (int i = 0; i < bm.Lib64.GetCount(); i++)
			lstLib64.Add(bm.Lib64[i]);
	}
	
private:
	Vector<BuildMethod>& methods;
};

class Zide: public WithZideLayout<TopWindow> {
public:
	typedef Zide CLASSNAME;

	MenuBar mnuMain;
	ToolBar tlbMain;
	Splitter splMain;
	SplitterFrame splBottom;
	SplitterFrame splExplore;
	SplitterFrame splOutput;
	WithExploreLayout<ParentCtrl> explore;
	ParentCtrl canvas;
	EditorManager tabs2;
	AssemblyBrowser asbAss;
	EditorManager tabs;
	Label lblLine;
	FrameTop<StaticBarArea> bararea;
	FrameBottom<Console> console;
	String lastPackage;
	String openDialogPreselect;
	Vector<String> openNodes;
	Index<String> recent;
	String openFile;
	SmartEditor edtDummy;
	
	bool running;
	int optimize;
	bool libMode;
	bool mirrorMode = false;
	bool toolbar_in_row;
	DropList lstBldConf;
	MultiButton mbtEntryPoint;
	MultiButton mbtBldMode;
	PopUpTable popMethodList;
	PopUpTable popTypeList;
	PopUpTable popArchList;
	StopWatch sw;
	Vector<String> packages;

	Settings settings;
	bool oShowPakPaths = true;
	String zcPath;
	
	RichTextCtrl annotation_popup;
	
	Zide();

	void DoMainMenu(Bar& bar);
	void DoMenuFile(Bar& bar);
	void DoMenuEdit(Bar& bar);
	void DoMenuFormat(Bar& bar);
	void DoMenuBuild(Bar& bar);
	void HelpMenu(Bar& bar);
	void DoMenuRecent(Bar& bar);
	void MainToolbar(Bar& bar);
	void DoMenuOptimize(Bar& bar);
	
	void OnMenuFormatShowSettings();

	void OnSelectSource();
	
	void OnMenuFileLoadPackage();
	void OnMenuFileLoadFile();
	void OnMenuFileSaveFile();
	void OnMenuFileSaveAll();
	void OnMenuShowPackagePaths();
	
	void OnMenuEditFind();
	void OnMenuEditReplace();
	void OnMenuEditFindNext();
	void OnMenuEditFindPrevious();
	
	void OnMenuBuildShowLog();
	void OnMenuBuildKill();
	void OnMenuBuildLibMode();
	void OnMenuBuildMethods();
	
	void OnMenuHelpAbout();
	void OnMenuHelpRebuildDocs();
	
	void OnFileRemoved(const String& file);
	void OnFileSaved(const String& file);
	bool OnRenameFiles(const Vector<String>& files, const String& oldPath, const String& newPath);
	void OoMenuRecent(const String& path);
	void OnToolO0();
	void OnToolO1();
	void OnToolO2();
	void OnSelectMethod();
	void OnAnnotation();

	void Serialize(Stream& s);
	void LoadModule(const String& mod, int color);

	void Load(Vector<String>& list, int id);
	void OnClose();
	void SetupLast();

	void OnMenuFormatMakeTabs();
	void OnMenuFormatMakeSpaces();
	void OnMenuFormatMakeLineEnds();
	void OnMenuFormatDuplicateLine();

	void OnEditChange();
	void OnEditCursor();
	void OnGoTo();
	void OnMenuBuildBuild();
	void OnMenuBuildFrontend();
	void OnMenuBuildRun(bool newConsole);
	void OnMenuBuildMirror();
	void OnOutputSel();
	void OnExplorerClick();
	void OnExplorerMenu(Bar& bar);
	void OnGenerateDocTemp();
	void OnGenerateDocTemp3(ZClass& cls, FileOut& f, FileOut& f2);
	void WriteDocEntry(FileOut& file, FileOut& f2, DocEntry& doc, Index<String>& links);
	
	void AddOutputLine(const String& str);
	void OutPutEnd();

	SmartEditor& GetEditor();
	OpenFileInfo* GetInfo();
	
	void LoadNavigation();
	void LoadNavigation(ZSource& source);
	void NavigationDone(ZSource* source, uint64 hash);

	String Build(const String& file, bool scu, bool& res);
	void ReadHlStyles(ArrayCtrl& hlstyle);
	
	void DropMethodList();
	void DropTypeList();
	void DropArchList();
	
	void LoadPackage(const String& pak);
	
	bool IsVerbose() const;
	void PutConsole(const char *s);
	void PutVerbose(const char *s);

private:
	bool editThread;
	bool pauseExplorer;
	void OnTabChange();
	Vector<BuildMethod> methods;
	String method;
	String arch;
	bool canBuild = true;
	
	String rundir;
	String target;
	String runarg;
	String header;
	String footer;
	String docPath;
	
	Vector<int> colors;
	
	bool GetLineOfError(int ln);
};

#endif
