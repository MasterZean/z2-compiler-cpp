#include "zide.h"

#include <z2clib/Source.h>
#include <z2clib/Scanner.h>

ZPackage pak;

void OutlineThread(Zide* zide, const String& data, uint64 hash) {
	try {
		ZSource* source = new ZSource();
		source->Package = &pak;
		pak.Path = zide->lastPackage;
		source->Data = data;
		source->AddStdClassRefs();
		
		Scanner scanner(*source, true);
		scanner.Scan();
		
		PostCallback(callback2(zide, &Zide::NavigationDone, source, hash));
	}
	catch (...) {
	}
}

void CreateZSyntax(One<EditorSyntax>& e, int kind) {
	CSyntax& s = e.Create<CSyntax>();
	s.SetHighlight(kind);
}

Zide::Zide() {
	toolbar_in_row = false;
	
	optimize = 0;
	libMode = false;
	running = false;
	
	CtrlLayout(*this, "ZIDE");
	Sizeable().Zoomable().Icon(ZImg::icon());
	
	int r = HorzLayoutZoom(100);
	int l = HorzLayoutZoom(250);
	mnuMain.Transparent();
	if (toolbar_in_row) {
		tlbMain.SetFrame(NullFrame());
		int tcy = tlbMain.GetStdHeight() + 1;
		bararea.Add(mnuMain.LeftPos(0, l).VCenterPos(mnuMain.GetStdHeight()));
		bararea.Add(tlbMain.HSizePos(l, r).VCenterPos(tcy));
		bararea.Add(lblLine.RightPos(4, r).VSizePos(2, 3));
		lblLine.AddFrame(ThinInsetFrame());
		bararea.Height(max(mnuMain.GetStdHeight(), tcy));
		AddFrame(bararea);
		tlbMain.Transparent();
		AddFrame(TopSeparatorFrame());
	}
	else {
		bararea.Add(mnuMain.LeftPos(0, l).VCenterPos(mnuMain.GetStdHeight()));
		bararea.Add(lblLine.RightPos(4, r).VSizePos(2, 3));
		lblLine.AddFrame(ThinInsetFrame());
		bararea.Height(mnuMain.GetStdHeight());
		AddFrame(bararea);
		AddFrame(TopSeparatorFrame());
		AddFrame(tlbMain);
		tlbMain.NoTransparent();
	}

	mnuMain.Set(THISBACK(DoMainMenu));
	tlbMain.Set(THISBACK(MainToolbar));
	
	Add(splMain);
	splMain.Horz(asbAss, canvas);
	splMain.SetPos(1750);
	
	canvas.Add(tabs);
	tabs.SizePos();
	
	console.Height(150);
	
	CtrlLayout(explore);
	explore.lstItems.SetDisplay(Single<ItemDisplay>());
	explore.lstItems.RenderMultiRoot();
	explore.lstItems.NoRoot();
	explore.lstItems.WhenAction = THISBACK(OnExplorerClick);
	explore.lstItems.Set(0, "aa", RawToValue(ZItem()));
	explore.lstItems.NoWantFocus();
	explore.lstItems.WhenBar = THISBACK(OnExplorerMenu);
	
	canvas.AddFrame(splBottom.Bottom(console, 150));
	canvas.AddFrame(splOutput.Right(tabs2, 400));
	tabs.AddFrame(splExplore.Left(explore, 200));
	splOutput.Hide();
	
	explore.AddFrame(LeftSeparatorFrame());
	
	splBottom.Hide();
	console.WhenLeft = THISBACK(OnOutputSel);

	asbAss.WhenSelectSource = THISBACK(OnSelectSource);
	asbAss.WhenFileRemoved = THISBACK(OnFileRemoved);
	asbAss.WhenFileSaved = THISBACK(OnFileSaved);
	asbAss.WhenRenameFiles = THISBACK(OnRenameFiles);

	WhenClose = THISBACK(OnClose);
	tabs.WhenEditorCursor = THISBACK(OnEditCursor);
	tabs.WhenEditorChange = THISBACK(OnEditChange);
	tabs.WhenTabChange = THISBACK(OnTabChange);
	tabs.WhenAnnotation = THISBACK(OnAnnotation);
	lblLine.SetAlign(ALIGN_CENTER);

	edtDummy.Enable(false);
	
	tabs.tabFiles.Hide();
	
	editThread = true;
	pauseExplorer = false;
	
	colors.Add(0);
	
	colors.Add(11);
	colors.Add(12);
	colors.Add(13);
	
	colors.Add(9);
	colors.Add(25);
	colors.Add(20);
	colors.Add(22);
	colors.Add(23);
	colors.Add(24);
	colors.Add(21);
	colors.Add(10);
	
	colors.Add(2);
	colors.Add(3);
	colors.Add(6);
	colors.Add(7);
	colors.Add(8);
	
	colors.Add(15);
	colors.Add(16);
	colors.Add(17);
	colors.Add(18);
	colors.Add(19);
	
	colors.Add(28);
	colors.Add(29);
	colors.Add(30);
	colors.Add(31);
	
	lstBldConf.Add("Default");
	lstBldConf.Tip("Select build configuration");
	lstBldConf.SetIndex(0);
	lstBldConf.NoDropFocus();
	lstBldConf.NoWantFocus();
	
	mbtBldMode.NoWantFocus();
	mbtBldMode.Tip("Build mode");
	mbtBldMode.AddButton().Tip("Build method").Left() <<= THISBACK(DropMethodList);
	mbtBldMode.AddButton().Tip("Build type") <<= THISBACK(DropTypeList);
	mbtBldMode.AddButton().Tip("Build architecture") <<= THISBACK(DropArchList);
	
	mbtEntryPoint.NoWantFocus();
	mbtEntryPoint.Tip("Current entry point");
	mbtEntryPoint.Set("* File in editor");
	mbtEntryPoint.AddButton().Tip("Set entry point").SetImage(ZImg::dots) <<= Callback()/*THISBACK(DropTypeList)*/;
	
	popMethodList.Normal();
	popMethodList.WhenSelect = THISBACK(OnSelectMethod);
	
	String curDir = NativePath(GetCurrentDirectory() + "\\");
	LoadFromXMLFile(methods, curDir + "buildMethods.xml");
	if (methods.GetCount() == 0) {
		methods.Clear();
		Cout() << "No cached build method found! Trying to auto-detect...\n";
		BuildMethod::Get(methods);
		if (methods.GetCount() == 0) {
			PromptOK("Could not find any build methods. Building is dissabled!");
			canBuild = false;
		}
		StoreAsXMLFile(methods, "methods", curDir + "buildMethods.xml");
	}
	
	for (int i = 0; i < methods.GetCount(); i++)
		popMethodList.Add(methods[i].Name);
	if (methods.GetCount())
		popMethodList.SetCursor(0);
	
	popTypeList.Normal();
	popTypeList.WhenSelect = THISBACK(OnSelectMethod);
	popTypeList.Add("Debug");
	popTypeList.Add("Speed");
	popTypeList.Add("Size");
	popTypeList.SetCursor(1);
	
	popArchList.Normal();
	popArchList.WhenSelect = THISBACK(OnSelectMethod);
	popArchList.Add("x86");
	popArchList.SetCursor(0);
	
	OnSelectMethod();
	
	annotation_popup.Background(White);
	annotation_popup.SetFrame(BlackFrame());
	annotation_popup.Margins(6);
	annotation_popup.NoSb();
	
	docPath = GetFileDirectory(GetExeFilePath());
	docPath << "docs\\pak\\";
	docPath = NativePath(docPath);
}

void Zide::DropMethodList() {
	popMethodList.PopUp(&mbtBldMode);
}

void Zide::DropTypeList() {
	popTypeList.PopUp(&mbtBldMode);
}

void Zide::DropArchList() {
	popArchList.PopUp(&mbtBldMode);
}

void Zide::OnSelectMethod() {
	String s;
	if (popMethodList.GetCursor() != -1) {
		method = popMethodList.Get(popMethodList.GetCursor(), 0);
		s << method;
	}
	
	int index = -1;
	for (int i = 0; i < methods.GetCount(); i++) {
		if (methods[i].Name == method) {
			index = i;
			break;
		}
	}
	
	if (popArchList.GetCursor() != -1) {
		String oldArch = popArchList.Get(popArchList.GetCursor(), 0);
		popArchList.Clear();
		
		if (index!= -1) {
			if (methods[index].Lib32.GetCount()) {
				popArchList.Add("x86");
				if (oldArch == "x86")
					popArchList.SetCursor(popArchList.GetCount() - 1);
			}
			if (methods[index].Lib64.GetCount()) {
				popArchList.Add("x64");
				if (oldArch == "x64")
					popArchList.SetCursor(popArchList.GetCount() - 1);
			}
		}
		
		if (popArchList.GetCursor() == -1 && popArchList.GetCount())
			popArchList.SetCursor(popArchList.GetCount() - 1);
		
		if (popArchList.GetCursor() != -1) {
			if (!s.IsEmpty())
				s << " ";
			arch = popArchList.Get(popArchList.GetCursor(), 0);
			s << arch;
		}
		else
			arch = "";
	}
	
	int c = popTypeList.GetCursor();
	if (c != -1) {
		if (!s.IsEmpty())
			s << " ";
		s << popTypeList.Get(c, 0);
		if (c == 0)
			OnToolO0();
		else if (c == 2)
			OnToolO1();
		else if (c == 1)
			OnToolO2();
	}
	
	mbtBldMode.Set(s);
}

void Zide::ReadHlStyles(ArrayCtrl& hlstyle) {
	CodeEditor& editor = GetEditor();
	
	hlstyle.Clear();
	for(int i = 0; i < colors.GetCount(); i++) {
		const HlStyle& s = editor.GetHlStyle(colors[i]);
		hlstyle.Add(editor.GetHlName(colors[i]), s.color, s.bold, s.italic, s.underline);
	}
}

void Zide::OnClose() {
	while (Thread::GetCount()) {
		Sleep(10);
	}
	
	if (tabs.PromptSaves())
		Close();
}

void Zide::OnFileRemoved(const String& file) {
	tabs.RemoveFile(file);
	mnuMain.Set(THISBACK(DoMainMenu));
}

void Zide::OnExplorerClick() {
	if (pauseExplorer)
		return;
	
	int i = explore.lstItems.GetCursor();
	if (i == -1)
		return;
	
	CodeEditor& editor = GetEditor();
	Point p = ValueTo<ZItem>(explore.lstItems.GetValue(i)).Pos;
	editThread = false;
	editor.SetCursor(editor.GetGPos(p.x - 1, p.y - 1));
	editor.SetFocus();
	editThread = true;
}

void Zide::OnExplorerMenu(Bar& bar) {
	int i = explore.lstItems.GetCursor();
	if (i == -1)
		return;

	ZItem zi = ValueTo<ZItem>(explore.lstItems.GetValue(i));
	if (zi.Kind == 1 || zi.Kind == 2)
		bar.Add("Generate documentation template",  THISBACK(OnGenerateDocTemp));
}

void Zide::OnFileSaved(const String& file) {
	int i = tabs.tabFiles.FindKey(file);
	if (i != -1)
		tabs.Save(i);
}

bool Zide::OnRenameFiles(const Vector<String>& files, const String& oldPath, const String& newPath) {
	for (int i = 0; i < files.GetCount(); i++) {
		int j = tabs.tabFiles.FindKey(files[i].ToWString());
		if (j != -1) {
			if (tabs.IsChanged(j)) {
				tabs.Save(j);
			}
		}
	}
	
	if (FileMove(oldPath, newPath))
		for (int i = 0; i < files.GetCount(); i++) {
			int j = tabs.tabFiles.FindKey(files[i].ToWString());
			if (j != -1) {
				ASSERT(files[i].StartsWith(oldPath));
				String np = newPath + files[i].Mid(oldPath.GetLength());
				WString w1 = tabs.tabFiles.GetKey(j);
				WString w2 = np.ToWString();
				int k = tabs.files.Find(w1);
				ASSERT(k != -1);
				OpenFileInfo* info = tabs.files.Detach(k);
				tabs.files.Add(w2);
				tabs.files.Set(tabs.files.GetCount() - 1, info);
				DUMP(w1);
				DUMP(w2);
				tabs.tabFiles.RenameFile(w1, w2, ZImg::zsrc);
			}
		}
		
	return true;
}

SmartEditor& Zide::GetEditor() {
	int i = tabs.tabFiles.GetCursor();
	if (i == -1)
		return edtDummy;
	WString file = tabs.tabFiles[i].key;
	int j = tabs.files.Find(file);
	if (j == -1)
		return edtDummy;

	return tabs.files[j].editor;
}

OpenFileInfo* Zide::GetInfo() {
	int i = tabs.tabFiles.GetCursor();
	if (i == -1)
		return nullptr;
	WString file = tabs.tabFiles[i].key;
	int j = tabs.files.Find(file);
	if (j == -1)
		return nullptr;
	return &tabs.files[j];
}

String Zide::Build(const String& file, bool scu, bool& res) {
	String cmd = zcPath;
	if (cmd.GetCount() == 0)
		cmd = BuildMethod::Exe("z2c");
	cmd << " -";
	if (scu)
		cmd << "scu ";
	else
		cmd << "c++ ";
	
	cmd << "-file " << file << " ";
	cmd << "-pak " << lastPackage << " ";
	if (optimize == 2)
		cmd << " -O2";
	else if (optimize == 1)
		cmd << " -O1";
	else if (optimize == 0)
		cmd << " -Od";

	if (libMode)
		cmd << " -lib";
	
	if (popMethodList.GetCursor() != -1)
		cmd << " -bm " << popMethodList.Get(popMethodList.GetCursor(), 0);
	
	cmd << " -arch " << arch;
	
	String t, tt;
	LocalProcess lp(cmd);

	while (lp.Read(t)) {
		if (t.GetCount())
			tt << t;
	}
	res = BuildMethod::IsSuccessCode(lp.GetExitCode());

	if (res == false && tt.GetCount() == 0) {
		cmd = GetFileDirectory(GetExeFilePath()) + BuildMethod::Exe("z2c");

		if (!FileExists(cmd))
			tt = "Could not find: " + cmd;
	}
	
	return tt;
}

void Zide::AddOutputLine(const String& str) {
	console << str;
	console.ScrollEnd();
}

bool Zide::IsVerbose() const {
	return console.verbosebuild;
}

void Zide::PutConsole(const char *s) {
	console << s << "\n";
}

void Zide::PutVerbose(const char *s) {
	if(console.verbosebuild) {
		PutConsole(s);
		console.Sync();
	}
}

void Zide::OutPutEnd() {
	console.ScrollLineUp();
	console.ScrollLineUp();
	Title("ZIDE - Execution done in " + sw.ToString() + " sec.");
	running = false;
}

void Zide::OnEditChange() {
	OpenFileInfo* info = GetInfo();
	if (!info)
		return;
	CodeEditor& editor = info->editor;
	if (!editor.IsEnabled())
		return;
	
	info->Hash++;
	if (editThread)
		Thread().Run(callback3(OutlineThread, this, editor.Get(), info->Hash));
}

void Zide::OnEditCursor() {
	OpenFileInfo* info = GetInfo();
	if (!info)
		return;
	
	CodeEditor& editor = info->editor;
	if (!editor.IsEnabled())
		return;
	
	Point p = editor.GetColumnLine(editor.GetCursor());
	lblLine.SetText(String().Cat() << "Ln " << (p.y + 1) << ", Cl " << (p.x + 1));
}

void Zide::NavigationDone(ZSource* source, uint64 hash) {
	OpenFileInfo* info = GetInfo();
	if (!info)
		return;
	
	if (info->Hash == hash)
		LoadNavigation(*source);
	
	delete source;
}

void Zide::OnTabChange() {
	int i = tabs.GetCursor();
	if (i == -1)
		return;
	
	openFile = tabs.tabFiles[i].key;
	
	i = asbAss.treModules.Find(tabs.tabFiles[i].key);
	if (i != -1) {
		asbAss.treModules.SetCursor(i);
	}
	
	tabs.tabFiles.Show(tabs.tabFiles.GetCount());
	splExplore.Show(tabs.tabFiles.GetCount());
}

void Zide::OnOutputSel() {
	if (console.GetSelection().GetLength() != 0)
		return;
	
	Point p = console.GetColumnLine(console.GetCursor());
	
	for (int i = 0; i < 5; i++) {
		if (p.y >= 0 &&	GetLineOfError(p.y))
			return;
		
		p.y--;
	}
}

bool Zide::GetLineOfError(int ln) {
	String line = console.GetUtf8Line(ln);
	
//#ifdef PLATFORM_WIN32
	int s = line.Find("(");
	int e = line.Find(")");
//#endif

/*#ifdef PLATFORM_POSIX
	int s = line.Find(":");
	int e = line.Find(": error: ");
#endif*/
	
	if (s > -1 && s < e) {
		String file = line.Left(s);

		if (FileExists(file)) {
			tabs.Open(file);
			
			String rest = line.Mid(s + 1, e - s - 1);
			Vector<String> v = Split(rest, ",");
			if (v.GetCount() == 2) {
				int x = StrInt(TrimBoth(v[0])) - 1;
				int y = StrInt(TrimBoth(v[1])) - 1;
				
				CodeEditor& editor = GetEditor();
				if (!editor.IsEnabled())
					return false;
				
				editor.SetCursor(editor.GetGPos(x, y));
				editor.SetFocus();
				
				return true;
			}
			else if (v.GetCount() == 1) {
				int x = StrInt(TrimBoth(v[0])) - 1;
				
				CodeEditor& editor = GetEditor();
				if (!editor.IsEnabled())
					return false;
				
				editor.SetCursor(editor.GetGPos(x, 1));
				editor.SetFocus();
			
				return true;
			}
			
			return false;
		}
	}
	
	return false;
}

void Zide::OnGoTo() {
	CodeEditor& editor = GetEditor();
	if (!editor.IsEnabled())
		return;
	static int line = 1;
	if (EditNumber(line, "Go to line numer", "Line:")) {
		editor.GotoLine(line - 1);
		editor.SetFocus();
	}
}

void Zide::OnSelectSource() {
	openFile = asbAss.GetItem();
	DUMP(openFile);
	
	tabs.Open(openFile);
	mnuMain.Set(THISBACK(DoMainMenu));
}

void Zide::Load(Vector<String>& list, int id) {
	for (int i = 0; i < asbAss.treModules.GetChildCount(id); i++) {
		int ch = asbAss.treModules.GetChild(id, i);
		if (asbAss.treModules.IsOpen(ch)) {
			String s = asbAss.treModules.GetNode(ch).key;
			list.Add(s);
		}
		Load(list, ch);
	}
}

void Zide::Serialize(Stream& s) {
	int version = 1;
	s / version;

	if (s.IsLoading()) {
		bool b;
		int w = 0;
		int h = 0;
		s % b % w % h;
		if (b)
			Maximize(b);
		else
			HCenterPos(w, 0).VCenterPos(h, 0);
		
		int split;
		s % split;
		splMain.SetPos(split);
	}
	else {
		bool b = IsMaximized();
		int w = GetSize().cx;
		int h = GetSize().cy;
		s % b % w % h;
		
		int split = splMain.GetPos();
		s % split;
	}
	
	s % openFile;
	s % lastPackage % openNodes % optimize % libMode % openDialogPreselect % recent;
	s % settings % method % arch % oShowPakPaths;
}

void Zide::LoadModule(const String& mod, int color) {
	asbAss.AddModule(NativePath(mod), color);
}

void Zide::SetupLast() {
	for (int i = 0; i < openNodes.GetCount(); i++) {
		int n = asbAss.treModules.Find(openNodes[i]);
		if (n != -1)
			asbAss.treModules.Open(n);
	}

	int n = asbAss.treModules.Find(openFile);
	if (n != -1)
		asbAss.treModules.SetCursor(n);

	tabs.tabFiles.Show(tabs.tabFiles.GetCount());
	splExplore.Show(tabs.tabFiles.GetCount());
	
	tabs.SetSettings(settings);
	
	tabs.tabFiles.SetAlign(settings.TabPos);
	tabs.tabFiles.Crosses(settings.TabClose >= 0, settings.TabClose);
	
	tlbMain.Set(THISBACK(MainToolbar));
	mnuMain.Set(THISBACK(DoMainMenu));
	
	int ii = popMethodList.Find(method);
	if (ii != -1) {
		popMethodList.SetCursor(ii);
		OnSelectMethod();
	}
	
	asbAss.SetShowPaths(oShowPakPaths);
}

void Zide::LoadPackage(const String& package) {
	if (package.GetCount() == 0)
		return;
	
#ifdef PLATFORM_WIN32
	String platform = "WIN32";
	String platformLib = "microsoft.windows";
#endif

			
#ifdef PLATFORM_POSIX
	String platform = "POSIX";
	String platformLib = "ieee.posix";
#endif

	lastPackage = package;
	asbAss.ClearModules();
	packages << lastPackage;
	LoadModule(lastPackage, 0);
	
	String s = GetFileDirectory(GetExeFilePath());
	String pak = s + "source/stdlib/sys.core";
	if (DirectoryExists(pak)) {
		packages << pak;
		LoadModule(pak, 1);
	}
	pak = s + "source/stdlib/bind.c";
	if (DirectoryExists(pak)) {
		packages << pak;
		LoadModule(pak, 1);
	}
	pak = s + "source/stdlib/" + platformLib;
	if (DirectoryExists(pak)) {
		packages << pak;
		LoadModule(pak, 2);
	}
	
	openNodes.Add(lastPackage);
	SetupLast();
	
	int i = recent.Find(lastPackage);
	if (i == -1)
		recent.Insert(0, lastPackage);
	else {
		recent.Remove(i);
		recent.Insert(0, lastPackage);
	}
}

GUI_APP_MAIN {
	SetLanguage(LNG_ENGLISH);
	SetDefaultCharset(CHARSET_UTF8);

	EditorSyntax::Register("z2", callback1(CreateZSyntax, CSyntax::HIGHLIGHT_Z2), "*.z2",	"Z2	Source Files");
	
	Ctrl::SetAppName("ZIDE");
	Zide zide;
	String zz;
	String curDir = GetFileDirectory(GetExeFilePath());
	
	FindFile ff(curDir + "/" + BuildMethod::Exe("z2c"));
	if (ff.IsExecutable())
		zide.zcPath = ff.GetPath();
		
	if (!LoadFromFile(zide)) {
		String s = curDir + "source/ut/org.z2legacy.ut";
		s = NativePath(s);
		if (DirectoryExists(s)) {
			zide.lastPackage = s;
			zide.openNodes.Add(s);
			zide.recent.Add(s);
			
			zz = s + "/Hello.z2";
			zide.openFile = NativePath(zz);
		}
	}
	
	zide.LoadPackage(zide.lastPackage);

	zide.Run();
	StoreToFile(zide);
}
