#include "zide.h"

struct FormatDlg : TabDlg {
	ColorPusher hl_color[CodeEditor::HL_COUNT];
};

void HlPusherFactory(One<Ctrl>& ctrl) {
	ctrl.Create<ColorPusher>().NotNull().Track();
}

LocalProcess globalExecutor;
void* globalProcesID;

void ExecutableThread(Zide* zide, const String& file, bool newConsole) {
	
#ifdef PLATFORM_WIN32

	if (newConsole) {
		int n = file.GetLength() + 1;
		Buffer<char> cmd(n);
		memcpy(cmd, file, n);
		
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		si.cb = sizeof(STARTUPINFO);
		
		if (CreateProcess(NULL, cmd, &sa, &sa, TRUE,
			             NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE,
		                NULL, NULL, &si, &pi)) {
		    globalProcesID = (void*)pi.hProcess;
		    
		    WaitForSingleObject(pi.hProcess, INFINITE);
		                    
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			
			globalProcesID = nullptr;
		}
		else
			;//PutConsole("Unable to launch " + String(_cmdline));
	}
	else {
		
#endif

		String t, tt;
		
		globalExecutor.Kill();
		globalExecutor.Start(file);
		//LocalProcess lp2(file);
		
		while (globalExecutor.Read(t)) {
			if (t.GetCount()) {
				PostCallback(callback1(zide, &Zide::AddOutputLine, t));
			}
		}
		
#ifdef PLATFORM_WIN32
	}
#endif
	
	PostCallback(callback(zide, &Zide::OutPutEnd));
}

void Zide::DoMainMenu(Bar& bar) {
	int i = tabs.tabFiles.GetCursor();
	bool tab = i != -1;
	
	bar.Add("File",       THISBACK(DoMenuFile));
	
	if (tab) {
		bar.Add("Edit",   THISBACK(DoMenuEdit));
		bar.Add("Format", THISBACK(DoMenuFormat));
		bar.Add("Build",  THISBACK(DoMenuBuild));
	}
	
	bar.Add("Help",       THISBACK(HelpMenu));
}

void Zide::DoMenuFile(Bar& bar) {
	int i = tabs.tabFiles.GetCursor();
	bool tab = i != -1;
	
	bar.Add("&New project", CtrlImg::new_doc(), Callback())
		.Key(K_CTRL | K_N).Enable(false);
	bar.Add("&New package", CtrlImg::MkDir(), Callback())
		.Key(K_CTRL | K_N).Enable(false);
	bar.Separator();
	
	bar.Add("&Open project", CtrlImg::open(), Callback())
		.Key(K_CTRL | K_O).Enable(false);
	bar.Add("&Open package", CtrlImg::open(), THISBACK(OnMenuFileLoadPackage))
		.Key(K_CTRL | K_O);
	bar.Add("&Open file",                    THISBACK(OnMenuFileLoadFile));
	bar.Add(tab, "&Save", CtrlImg::save(),   THISBACK(OnMenuFileSaveFile))
		.Key(K_CTRL | K_S);
	bar.Add("&Save all",                     THISBACK(OnMenuFileSaveAll));
	bar.Separator();
	
	if (recent.GetCount())
		bar.Add("&Recent packages",           THISBACK(DoMenuRecent));
	else
		bar.Add("&Recent packages",           Callback()).Enable(false);
	bar.Separator();
	
	bar.Add("Show package paths",                     THISBACK(OnMenuShowPackagePaths))
		.Check(oShowPakPaths);
	bar.Separator();
	
	bar.Add("E&xit",                         THISBACK(Close))
		.Key(K_ALT | K_F4);
}

void Zide::OnMenuFileLoadPackage() {
	FileSel fs;
	fs.PreSelect(lastPackage);
	
	if (fs.ExecuteSelectDir()) {
		if (!tabs.PromptSaves())
			return;
			
		LoadPackage(fs.Get());
	}
}

void Zide::OnMenuFileLoadFile() {
	FileSel fs;
	fs.PreSelect(openDialogPreselect);
	if (fs.ExecuteOpen()) {
		openDialogPreselect = fs.Get();
		tabs.Open(openDialogPreselect);
	}
}

void Zide::OnMenuFileSaveFile() {
	tabs.Save(tabs.GetCursor());
}

void Zide::OnMenuFileSaveAll() {
	tabs.SaveAll();
}

void Zide::OnMenuShowPackagePaths() {
	oShowPakPaths = !oShowPakPaths;
	asbAss.SetShowPaths(oShowPakPaths);
}

void Zide::DoMenuEdit(Bar& bar) {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	bool selection = editor.IsAnySelection();

	bar.Add("Undo", CtrlImg::undo(),             callback(&editor, &LineEdit::Undo))
		.Key(K_CTRL_Z)
		.Enable(editor.IsUndo())
		.Help("Undo changes to text");
	bar.Add("Redo", CtrlImg::redo(),             callback(&editor, &LineEdit::Redo))
		.Key(K_SHIFT|K_CTRL_Z)
		.Enable(editor.IsRedo())
		.Help("Redo undone changes");
	bar.Separator();
	
	bar.Add("Cut", CtrlImg::cut(),               callback(&editor, &LineEdit::Cut))
		.Key(K_CTRL_X)
		.Enable(selection)
		.Help("Cut selection and place it on the system clipboard");
	bar.Add("Copy", CtrlImg::copy(),             callback(&editor, &LineEdit::Copy))
		.Key(K_CTRL_C)
		.Enable(selection)
		.Help("Copy current selection on the system clipboard");
	bar.Add("Paste", CtrlImg::paste(),           callback(&editor, &LineEdit::Paste))
		.Key(K_CTRL_V)
		.Help("Insert text from clipboard at cursor location");
	bar.Separator();
	bar.Add("Select all", CtrlImg::select_all(), callback(&editor, &LineEdit::SelectAll))
		.Key(K_CTRL_A);
	bar.Separator();
	
	bar.Add("Go to line...",                     THISBACK(OnGoTo)).Key(K_CTRL | K_G);
	bar.Separator();
	
	bar.Add("Find...",                           THISBACK(OnMenuEditFind))
		.Help("Search for text or text pattern")
		.Key(K_CTRL | K_F);
	bar.Add("Replace...",                        THISBACK(OnMenuEditReplace))
		.Help("Search for text or text pattern, with replace option")
		.Key(K_CTRL | K_H);
	bar.Add("Find next",                         THISBACK(OnMenuEditFindNext))
		.Help("Find next occurrence")
		.Key(K_F3);
	bar.Add("Find previous",                     THISBACK(OnMenuEditFindPrevious))
		.Help("Find previous occurrence")
		.Key(K_SHIFT | K_F3);
}

void Zide::OnMenuEditFind() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.FindReplace(false, false, false);
}

void Zide::OnMenuEditReplace() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.FindReplace(false, false, true);
}

void Zide::OnMenuEditFindNext() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.FindNext();
}

void Zide::OnMenuEditFindPrevious() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.FindPrev();
}

void Zide::DoMenuFormat(Bar& menu) {
	menu.Add("Settings...",         THISBACK(OnMenuFormatShowSettings));
	menu.Separator();
	
	menu.Add("Spaces to tabs",      THISBACK(OnMenuFormatMakeTabs))
		.Help("Convert leading blanks on each line to tabs");
	menu.Add("Tabs to spaces",      THISBACK(OnMenuFormatMakeSpaces))
		.Help("Convert all tabs to spaces");
	menu.Add("Trim line endings",   THISBACK(OnMenuFormatMakeLineEnds))
		.Help("Remove tabs and spaces at line endings");
	menu.Add("Duplicate line",      THISBACK(OnMenuFormatDuplicateLine))
	    .Help("Duplice the current line").Key(K_CTRL | K_D);
}

void Zide::OnMenuFormatShowSettings() {
	FormatDlg dlg;
	dlg.Title("Settings").Sizeable();
	dlg.SetMinSize(Size(602, 413));
	
	WithSetupHlLayout<ParentCtrl> hlt;
	WithSetupEditorLayout<ParentCtrl> edt;
	
	edt.filetabs
		.Add(AlignedFrame::LEFT, "Left")
		.Add(AlignedFrame::TOP, "Top")
		.Add(AlignedFrame::RIGHT, "Right")
		.Add(AlignedFrame::BOTTOM, "Bottom")
		.Add(-1, "Off");
		
	edt.tabs_crosses
		.Add(AlignedFrame::LEFT, "Left")
		.Add(AlignedFrame::RIGHT, "Right")
		.Add(-1, "Off");
		
	edt.tabsize.MinMax(1, 100).NotNull();
	edt.optScope.SetVertical();
	edt.optBracket.SetVertical();
	
	CtrlRetriever rtvr;
	rtvr
		(edt.showtabs, settings.ShowTabs)
		(edt.showspaces, settings.ShowSpaces)
		(edt.shownewlines, settings.ShowNewlines)
		(edt.warnwhitespace, settings.WarnSpaces)
		(edt.tabsize, settings.TabSize)
		(edt.indentspaces, settings.IndentSpaces)
		(edt.ShowLineNums, settings.ShowLineNums)
		(edt.HighlightLine, settings.HighlightLine)
		(edt.filetabs, settings.TabPos)
		(edt.tabs_crosses, settings.TabClose)
		(edt.LinePos, settings.LinePos)
		(edt.LineColor, settings.LineColor)
		(edt.optScope, settings.ScopeHighlight)
		(edt.optBracket, settings.Brackets)
		(edt.optThousands, settings.Thousands);
	
	hlt.arcStyle.AddColumn("Style");
	hlt.arcStyle.AddColumn("Color").Ctrls(HlPusherFactory);
	hlt.arcStyle.AddColumn("Bold").Ctrls<Option>();
	hlt.arcStyle.AddColumn("Italic").Ctrls<Option>();
	hlt.arcStyle.AddColumn("Underline").Ctrls<Option>();
	hlt.arcStyle.ColumnWidths("211 80 45 45 80");
	hlt.arcStyle.EvenRowColor().NoHorzGrid().SetLineCy(EditField::GetStdHeight() + 2);
	ReadHlStyles(hlt.arcStyle);
	
	hlt.lstLanguage.Add("Z2");
	hlt.lstLanguage.SetIndex(0);
		
	rtvr <<= dlg.Breaker(222);
	hlt.arcStyle.WhenCtrlsAction = dlg.Breaker(222);
	hlt.btnRestore <<= dlg.Breaker(223);
	
	dlg.Add(edt, "Editor");
	dlg.Add(hlt, "Syntax highlighting");

	dlg.WhenClose = dlg.Acceptor(IDEXIT);
	
	for(;;) {
		int c = dlg.Run();
		
		Size sz = dlg.GetSize();
		
		if (c == 223) {
			CodeEditor::DefaultHlStyles();
			ReadHlStyles(hlt.arcStyle);
		}
		
		rtvr.Retrieve();
		
		CodeEditor& editor = GetEditor();
		int i = tabs.tabFiles.GetCursor();
		
		tabs.tabFiles.SetAlign(settings.TabPos);
		tabs.tabFiles.Crosses(settings.TabClose >= 0, settings.TabClose);
		
		for(int i = 0; i < colors.GetCount(); i++) {
			int j = colors[i];
			editor.SetHlStyle(j, hlt.arcStyle.Get(i, 1), hlt.arcStyle.Get(i, 2),
			                     hlt.arcStyle.Get(i, 3), hlt.arcStyle.Get(i, 4));
		}
		
		WString file = tabs.tabFiles[i].key;
		EditorManager::SetSettings(editor, settings, EditorSyntax::GetSyntaxForFilename(file.ToString()));
		
		if(c == IDEXIT)
			break;
	}
	
	settings.Style = CodeEditor::StoreHlStyles();
	tabs.SetSettings(settings);
}

void Zide::OnMenuFormatMakeTabs() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	editor.MakeTabsOrSpaces(true);
	tabs.SetChanged(tabs.GetCursor(), true);
}

void Zide::OnMenuFormatMakeSpaces() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.MakeTabsOrSpaces(false);
	tabs.SetChanged(tabs.GetCursor(), true);
}

void Zide::OnMenuFormatMakeLineEnds() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.MakeLineEnds();
	tabs.SetChanged(tabs.GetCursor(), true);
}

void Zide::OnMenuFormatDuplicateLine() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	editor.DuplicateLine();
	tabs.SetChanged(tabs.GetCursor(), true);
}

void Zide::DoMenuBuild(Bar& bar) {
	if (running) {
		bar.Add("Kill current process",  THISBACK(OnMenuBuildKill))
			.Key(K_CTRL | K_F5);
		bar.Separator();
	}
	
	bar.Add("Compile && run current file in console",  THISBACK1(OnMenuBuildRun, false))
		.Key(K_F5)
		.Enable(!running && canBuild);
	bar.Add("Compile && run current file in new console",  THISBACK1(OnMenuBuildRun, true))
		.Key(K_CTRL | K_F5)
		.Enable(!running && canBuild);
	bar.Separator();
	
	bar.Add("Compile current file", CtrlImg::Toggle(), THISBACK(OnMenuBuildBuild))
		.Key(K_F7)
		.Enable(!running && canBuild);
	bar.Add("Frontend only compile current file",      THISBACK(OnMenuBuildFrontend))
		.Key(K_F8)
		.Enable(!running && canBuild);
	bar.Separator();
	
	bar.Add("Mirror mode",                             THISBACK(OnMenuBuildMirror))
		.Check(mirrorMode);
	bar.Separator();
	
	bar.Add("Build methods...",                        THISBACK(OnMenuBuildMethods));
	bar.Separator();
	bar.Add("Optimize backend code",                   THISBACK(DoMenuOptimize));
	bar.Add("Library mode",                            THISBACK(OnMenuBuildLibMode))
		.Check(libMode);
	bar.Separator();
	
	bar.Add("Show log",                                THISBACK(OnMenuBuildShowLog))
		.Check(splBottom.IsShown());
}

void Zide::OnMenuBuildRun(bool newConsole) {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	String file = tabs.tabFiles[tabs.GetCursor()].key;
	file = NativePath(file);
	tabs.SaveAllIfNeeded();
	
	editor.ClearErrors();
	editor.RefreshLayoutDeep();
	
	splBottom.Show();

	bool res = false;
	String t, tt;
	console.Set(String());
	tt = Build(file, true, res);

	if (res) {
		String ename = GetFileDirectory(file) + GetFileTitle(file) + BuildMethod::Exe("");
		
		if (!newConsole)
			console.Set(tt + "Running " + ename + "\n");
		else
			console.Set(tt);
		Title("ZIDE - Executing: " + ename);
		
		sw.Reset();
		running = true;
		
		Thread().Run(callback3(ExecutableThread, this, ename, newConsole));
	}
	else {
		console.Set(tt);
		console.ScrollEnd();
		
		int errors = 0;
		Vector<String> lines = Split(tt, '\n');
		for (int i = 0; i < lines.GetCount(); i++) {
			String s = TrimBoth(lines[i]);
			
			int ii = s.Find('(');
			if (ii != -1) {
				int jj = s.Find(')', ii);
				if (ii < jj) {
					String path = s.Mid(0, ii);
					if (path == file) {
						String ll = s.Mid(ii + 1, jj - ii - 1);
						Vector<String> s2 = Split(ll, ',');
						if (s2.GetCount() == 2) {
							int line = StrInt(s2[0]);
							editor.SetError(line - 1, 1);
							errors++;
						}
					}
				}
			}
		}
		
		if (errors)
			editor.RefreshLayoutDeep();
	}
}

void Zide::OnMenuBuildBuild() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	tabs.SaveAllIfNeeded();
	splBottom.Show();
	bool res;
	console.Set(Build(tabs.tabFiles[tabs.GetCursor()].key, true, res));
	
	console.ScrollEnd();
	console.ScrollLineUp();
}

void Zide::OnMenuBuildFrontend() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	
	tabs.SaveAllIfNeeded();
	splBottom.Show();
	bool res;
	String file = tabs.tabFiles[tabs.GetCursor()].key;
	console.Set(Build(file, false, res));
	
	console.ScrollEnd();
	console.ScrollLineUp();
	
	if (res && mirrorMode) {
		splOutput.Show();
		
		Point p = Point(-1, -1);
		Point p2 = Point(-1, -1);
		if (tabs2.GetCount()) {
			CodeEditor& editor = tabs2.files[0].editor;
			if (!editor.IsEnabled())
				return;
			
			p = editor.GetColumnLine(editor.GetCursor());
			p2 = editor.GetScrollPos();
		}
		
		String cfile = ForceExt(file, ".cpp");
		tabs2.Open(cfile, true);
		
		if (p.x >= 0) {
			CodeEditor& editor = tabs2.files[0].editor;
			editor.SetCursor(editor.GetGPos(p.y, p.x));
			editor.SetScrollPos(p2);
		}
	}
}

void Zide::OnMenuBuildMethods() {
	BuildMethodsWindow bmw(methods);
	bmw.Run(true);
}

void Zide::OnMenuBuildShowLog() {
	splBottom.Show(!splBottom.IsShown());
}

void Zide::OnMenuBuildLibMode() {
	libMode = !libMode;
}

void Zide::OnMenuBuildMirror() {
	mirrorMode = !mirrorMode;
	
	if (mirrorMode == false) {
		splOutput.Hide();
	}
}

void Zide::OnMenuBuildKill() {
#ifdef PLATFORM_WIN32
	if (globalProcesID) {
		TerminateProcess((HANDLE)globalProcesID, -1);
		globalProcesID = nullptr;
	}
	else {
		globalExecutor.Kill();
	}
#else
	globalExecutor.Kill();
#endif
	
	console << "Process killed!";
	console.ScrollEnd();
	
	running = false;
}

void Zide::DoMenuRecent(Bar& bar) {
	int m = min(recent.GetCount(), 9);
	for (int i = 0; i < m; i++) {
		bar.Add("&" + IntStr(i + 1) + "       " + recent[i], THISBACK1(OoMenuRecent, recent[i]));
	}
}

void Zide::OoMenuRecent(const String& path) {
	LoadPackage(path);
	
	int i = recent.Find(lastPackage);
	if (i == -1)
		recent.Insert(0, lastPackage);
	else {
		recent.Remove(i);
		recent.Insert(0, lastPackage);
	}
}

void Zide::DoMenuOptimize(Bar& bar) {
	bar.Add("Debug Optimization (-Od)", THISBACK(OnToolO0))
		.Check(optimize == 0);
	bar.Add("Low Optimization (-O1)",   THISBACK(OnToolO1))
		.Check(optimize == 1);
	bar.Add("High Optimization (-O2)",  THISBACK(OnToolO2))
		.Check(optimize == 2);
}

void Zide::HelpMenu(Bar& bar) {
	bar.Add("Generate MD documentation", THISBACK(OnMenuHelpRebuildDocs));
	bar.Separator();
	bar.Add("About", THISBACK(OnMenuHelpAbout));
}

void Zide::OnMenuHelpAbout() {
	PromptOK("zide!!!!!!!!!!!!!!!!!!!!!!");
}

void Zide::MainToolbar(Bar& bar) {
	if (toolbar_in_row)
		bar.Separator();
	
	bar.Add(mbtEntryPoint, HorzLayoutZoom(180));
	bar.Gap(4);
	bar.Add(lstBldConf, HorzLayoutZoom(120));
	bar.Gap(4);
	bar.Add(mbtBldMode, HorzLayoutZoom(180));
	bar.Gap(4);
	
	bar.Add("Override: Debug Optimization", ZImg::Od(), THISBACK(OnToolO0))
		.Check(optimize == 0);
	bar.Add("Override: Low Optimization", ZImg::O1(),   THISBACK(OnToolO1))
		.Check(optimize == 1);
	bar.Add("Override: High Optimization", ZImg::O2(),  THISBACK(OnToolO2))
		.Check(optimize == 2);
}

void Zide::OnToolO0() {
	optimize = 0;
	tlbMain.Set(THISBACK(MainToolbar));
}

void Zide::OnToolO1() {
	optimize = 1;
	tlbMain.Set(THISBACK(MainToolbar));
}

void Zide::OnToolO2() {
	optimize = 2;
	tlbMain.Set(THISBACK(MainToolbar));
}
