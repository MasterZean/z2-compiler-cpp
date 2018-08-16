#include "zide.h"

void DarkTheme() {
	HighlightSetup::SetHlStyle(HighlightSetup::INK_NORMAL, Color(255, 251, 247), true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_NORMAL, Color(51, 51, 51));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_SELECTED, White);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_SELECTED, Color(0, 120, 215));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DISABLED, Color(109, 109, 109));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_READONLY, Color(91, 91, 91));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_COMMENT, Color(134, 134, 134), false, true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_COMMENT_WORD, Blue, true, true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_COMMENT_WORD, Yellow);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_STRING, Color(238, 222, 130));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_OPERATOR, Color(249, 167, 40), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_KEYWORD, Color(254, 95, 146), true, true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_UPP, Color(188, 234, 100), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_SLOT, Color(188, 234, 100), true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_LNG, Color(59, 75, 78));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_ERROR, LtRed);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_PAR0, White, true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_PAR1, Color(233, 126, 117), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_PAR2, Color(61, 124, 61), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_PAR3, Color(143, 58, 143), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_INT, Color(205, 170, 249), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_FLOAT, Color(0xE2, 0x7F, 0xC8), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_HEX, Color(0xE2, 0x7F, 0xC8), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_OCT, Color(0xE2, 0x7F, 0xC8), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_CONST_STRINGOP, Color(205, 170, 249));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BRACKET0, Color(127, 127, 127), true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BRACKET, Color(197, 127, 127), true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BLOCK1, Color(35, 35, 35));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BLOCK2, Color(44, 31, 65));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BLOCK3, Color(18, 55, 2));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_BLOCK4, Color(78, 78, 3));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_MACRO, White, true, true);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_MACRO, Color(71, 71, 71));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_IFDEF, Color(73, 73, 73));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_IFDEF, Color(170, 170, 170));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_UPPER, Color(28, 255, 0));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_SQLBASE, Color(113, 255, 255), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_SQLFUNC, Color(113, 255, 255), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_SQLBOOL, Color(113, 255, 255), true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_UPPMACROS, Color(255, 127, 255));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_UPPLOGS, Green);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DIFF_FILE_INFO, Black, true);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DIFF_HEADER, Color(28, 127, 200));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DIFF_ADDED, Color(28, 42, 255));
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DIFF_REMOVED, LtRed);
	HighlightSetup::SetHlStyle(HighlightSetup::INK_DIFF_COMMENT, Color(80, 255, 80));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_SELWORD, Yellow);
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_ERROR, Color(226, 42, 0));
	HighlightSetup::SetHlStyle(HighlightSetup::PAPER_WARNING, Color(255, 255, 205));
	HighlightSetup::SetHlStyle(HighlightSetup::SHOW_LINE, Color(67, 67, 67));
	HighlightSetup::SetHlStyle(HighlightSetup::WHITESPACE, Color(126, 186, 234));
	HighlightSetup::SetHlStyle(HighlightSetup::WARN_WHITESPACE, Color(191, 126, 126));
}

EditorManager::EditorManager() {
	AddFrame(tabFiles);
	Add(edtCode);
	edtCode.HSizePos().VSizePos();

	tabFiles.StyleDefault().Write().Variant1Crosses();
	tabFiles.WhenAction = THISBACK(OnTabChange);
	tabFiles.CancelClose = THISBACK(OnCancelClose);
	tabFiles.CancelCloseSome = THISBACK(OnCancelCloseSome);
	tabFiles.WhenClose = THISBACK(OnTabClose);
	tabFiles.WhenCloseSome = THISBACK(OnTabCloseSome);
}

bool EditorManager::OnCancelClose(Value val) {
	WString w = val;
	int i = tabFiles.FindKey(w);

	if (i == -1)
		return true;

	if (IsChanged(i)) {
		BeepQuestion();
		String fn = DeQtf(tabFiles[i].key.ToString());
		int save = PromptSaveDontSaveCancel("[ph The following file was changed since last save:&-|[* " + fn + "]&Would you like to save it?]");
		if (save == 1)
			Save(i);
		else if (save == -1)
			return true;
	}
		
	return false;
}

void EditorManager::OnTabCloseSome(ValueArray arr) {
	for (int i = 0; i < arr.GetCount(); i++) {
		OnTabClose(arr[i]);
	}
}

bool EditorManager::OnCancelCloseSome(ValueArray arr) {
	Vector<String> files;
	Vector<int> indices;
	
	for (int i = 0; i < arr.GetCount(); i++) {
		WString w = arr[i];
		int j = tabFiles.FindKey(w);
		
		if (j != -1 && IsChanged(j)) {
			files.Add(tabFiles[j].key.ToString());
			indices.Add(j);
		}
	}
	
	String fileList;
	if (files.GetCount()) {
		if (files.GetCount() == 1)
			fileList = "&-|[* " + DeQtf(files[0]) + "]";
		else {
			fileList = "&-|[* " + DeQtf(files[0]) + "]";
			for (int j = 1; j < files.GetCount(); j++)
				fileList << "&-|[* " << DeQtf(files[j]) << "]";
		}
		
		BeepQuestion();
		int save = PromptSaveDontSaveCancel("[ph The following file was changed since last save:" + fileList + "&Would you like to save it?]");
		if (save == 1) {
			for (int k = 0; k < indices.GetCount(); k++)
				Save(indices[k]);
		}
		else if (save == -1)
			return true;
	}
	
	return false;
}

void EditorManager::OnTabClose(Value val) {
	WString w = val;
	int j = files.Find(w);
	if (j != -1)
		files.Remove(j);
}

void EditorManager::OnEditorChange() {
	SetChanged(tabFiles.GetCursor(), true);
	WhenEditorChange();
}

void EditorManager::OnTabChange() {
	WString file = tabFiles[tabFiles.GetCursor()].key;
	for (int j = 0; j < files.GetCount(); j++)
		files[j].editor.Hide();
	int i = files.Find(file);
	if (i != -1)
		files[i].editor.Show();
	WhenTabChange();
	WhenEditorChange();
}

bool EditorManager::IsChanged(int i) {
	if (i < 0 || i > tabFiles.GetCount())
		return false;

	return GetInfo(i).IsChanged;
}

void EditorManager::Save(int i) {
	if (i < 0 || i > tabFiles.GetCount())
		return;

	int j = files.Find(tabFiles[i].key);
	if (j == -1)
		return;

	SaveFile(files.GetKey(j).ToString(), files[j].editor.Get());
	SetChanged(i, false);
}

void EditorManager::SaveAll() {
	for (int i = 0; i < tabFiles.GetCount(); i++)
		Save(i);
}

void EditorManager::SaveAllIfNeeded() {
	for (int i = 0; i < tabFiles.GetCount(); i++)
		if (IsChanged(i))
			Save(i);
}

bool EditorManager::PromptSaves() {
	for (int i = 0; i < tabFiles.GetCount(); i++) {
		OpenFileInfo& info = GetInfo(i);
		if (info.IsChanged) {
			BeepQuestion();
			int save = PromptSaveDontSaveCancel("[ph The following file was changed since last save:&-|[* " + DeQtf(tabFiles[i].key.ToString()) + "]&Would you like to save it?]");
			if (save == 1)
				Save(i);
			else if (save == -1)
				return false;
		}
	}

	return true;
}

void EditorManager::SetChanged(int i, bool changed) {
	if (i < 0 || i > tabFiles.GetCount())
		return;

	GetInfo(i).IsChanged = changed;
	if (changed)
		tabFiles.SetColor(i, Color(255, 127, 127));
	else
		tabFiles.SetColor(i, Null);
}

OpenFileInfo& EditorManager::GetInfo(int i) {
	WString w = tabFiles[i].key;

	int j = files.Find(tabFiles[i].key);
	return files[j];
}

void EditorManager::OnEditorCursor() {
	WhenEditorCursor();
}

void EditorManager::OnAnnotation() {
	WhenAnnotation();
}

void EditorManager::Open(const String& item, bool forceReload) {
	if (!FileExists(item)) {
		ErrorOK("[pd Could not open file:&]-|[* " + DeQtf(item) + "]");
		return;
	}
	
	int i = tabFiles.FindKey(item);
	if (i == -1) {
		WString w = item.ToWString();
		tabFiles.AddFile(w, ZImg::zsrc());
		
		int ii = files.Find(w);
		if (ii == -1) {
			OpenFileInfo& ff = files.Add(w);
			ii = files.GetCount() - 1;
			String content = LoadFile(item);
			ff.editor.Set(content);
		}
		
		OpenFileInfo& f = files[ii];
		if (settings.Theme == 0)
			f.editor.WhiteTheme();
		else if (settings.Theme == 1)
			DarkTheme();
		f.editor.SetBarLight(settings.Theme == 0);
		
		f.editor.AddFrame(RightSeparatorFrame());
		String syn = EditorSyntax::GetSyntaxForFilename(item);
		f.editor.WhenAction = THISBACK(OnEditorChange);
		f.editor.WhenSel = THISBACK(OnEditorCursor);
		f.editor.WhenPopup = WhenPopup;
		f.editor.HiliteIfEndif(true);
		f.editor.CheckEdited();
		f.editor.HiliteScope(2);
		f.editor.Annotations(Zx(10));
		edtCode.Add(f.editor);
		f.editor.HSizePos().VSizePos();
		WhenEditorCursor();
		f.editor.SetFocus();
		SetSettings(f.editor, settings, syn);
		WhenEditorChange();
		
		f.editor.WhenAssistChange = THISBACK(OnAssistChange);
		f.editor.WhenAnnotationMove = THISBACK(OnAnnotation);
	}
	else {
		WString w2 = item.ToWString();
		int j = files.Find(w2);
		if (j != -1) {
			OpenFileInfo& f = files[j];
			tabFiles.SetCursor(i);
			f.editor.SetFocus();
			
			if (forceReload) {
				String content = LoadFile(item);
				f.editor.Set(content);
			}
		}
	}
}

void EditorManager::OnAssistChange(SmartEditor* e) {
	SetChanged(tabFiles.GetCursor(), true);
}

void EditorManager::RemoveFile(const String& item) {
	files.RemoveKey(item.ToWString());
	int ki = tabFiles.FindKey(item);
	tabFiles.CloseForce(ki);
}

void EditorManager::SetSettings(CodeEditor& editor, Settings& settings, const String& syntax) {
	editor.ShowTabs(settings.ShowTabs);
	editor.ShowSpaces(settings.ShowSpaces);
	editor.ShowLineEndings(settings.ShowNewlines);
	editor.WarnWhiteSpace(settings.WarnSpaces);
	editor.TabSize(settings.TabSize);
	editor.IndentSpaces(settings.IndentSpaces);
	editor.LineNumbers(settings.ShowLineNums);
	editor.ShowCurrentLine(settings.HighlightLine ? HighlightSetup::GetHlStyle(HighlightSetup::SHOW_LINE).color : (Color)Null);
	editor.BorderColumn(settings.LinePos, settings.LineColor);
	editor.HiliteScope(settings.ScopeHighlight);
	editor.HiliteBracket(settings.Brackets);
	editor.ThousandsSeparator(settings.Thousands);
	editor.Highlight(syntax);
}

void EditorManager::SetSettings(Settings& settings) {
	CodeEditor::LoadHlStyles(settings.Style);
	
	for (int i = 0; i < files.GetCount(); i++)
		SetSettings(files[i].editor, settings, EditorSyntax::GetSyntaxForFilename(files.GetKey(i).ToString()));
	
	this->settings = settings;
}

void EditorManager::SetColors(int colors) {
	for (int i = 0; i < files.GetCount(); i++) {
		if (colors == 0) {
			files[i].editor.WhiteTheme();
			files[i].editor.SetBarLight(true);
		}
		else if (colors == 1) {
			DarkTheme();
			files[i].editor.SetBarLight(false);
		}
	}
	
}

void SmartEditor::LeftDown(Point p, dword flags) {
	if (popUp.IsOpen())
		popUp.Close();
	CodeEditor::LeftDown(p, flags);
}

void SmartEditor::Update(const String& w) {
	exactMatch = false;
	popUp.Clear();
	
	String b = ToUpper(w);
	
	for (int i = 0; i < words.GetCount(); i++) {
		String a = ToUpper(words[i].Name);
		
		if (words[i].Name == w) {
			exactMatch = true;
			popUp.Add(RawToValue(words[i]));
		}
		else if (a.StartsWith(b))
			popUp.Add(RawToValue(words[i]));
	}
		
	if (w.GetLength() && popUp.GetCount())
		popUp.SetCursor(0);
}

int GetWord(const String& line, String& wrd, const Point& p) {
	int ofss = 0;
	
	if (p.x < line.GetLength() && IsAlNum(line[p.x])) {
		int b = p.x;
		while (b > 0 && IsAlNum(line[b - 1]))
			b--;
		int e = p.x;
		while (e < line.GetCount() - 1 && IsAlNum(line[e + 1]))
			e++;
		
		wrd = line.Mid(b, e - b + 1);
		ofss = p.x - b;
	}
	else if (p.x > 0 && p.x - 1< line.GetLength() && IsAlNum(line[p.x - 1])) {
		int b = p.x - 1;
		while (b > 0 && IsAlNum(line[b - 1]))
			b--;
		int e = p.x - 1;
		while (e < line.GetCount() - 1 && IsAlNum(line[e + 1]))
			e++;
		
		wrd = line.Mid(b, e - b + 1);
		ofss = p.x - b;
	}
	
	return ofss;
}

bool SmartEditor::Key(dword key, int count) {
	Point p = GetColumnLine(GetCursor());
		
	String line = GetUtf8Line(p.y);
	line.Replace("\t", "    ");
	
	String wrd = "";
	int ofss = ::GetWord(line, wrd, p);
	
	Point anch;
	
	if (key & K_CTRL) {
		int k = key & ~K_CTRL;
		if (k == K_SPACE) {
			String line = GetUtf8Line(p.y);
			line.Replace("\t", "    ");
			
			String wrd = "";
			int ofss = ::GetWord(line, wrd, p);
	
			anch.x = p.x - 1 - ofss;
			anch.y = p.y;
			
			Anchor = anch;
			
			WhenPopup();
			
			popUp.Clear();
			if (wrd.GetLength())
				Update(wrd);
			else {
				exactMatch = false;
				for (int i = 0; i < words.GetCount(); i++)
					popUp.Add(RawToValue(words[i]));
			}
			
			DoPopup(p);

			return true;
		}
		else if (k == K_DOWN) {
			int index = popUp.GetCursor();
			if (index != -1) {
				index++;
				if (index >= popUp.GetCount())
					index = 0;
				
				popUp.SetCursor(index);
				
				return true;
			}
		}
		else if (k == K_UP) {
			int index = popUp.GetCursor();
			if (index != -1) {
				index--;
				if (index < 0)
					index = popUp.GetCount() - 1;
				
				popUp.SetCursor(index);
				
				return true;
			}
		}
		else if (k == K_ENTER) {
			Feed(wrd, ofss);
			return true;
		}
				
		return CodeEditor::Key(key, count);
	}
	
	if (key == K_F1 || key == K_F2 || key == K_F3 || key == K_F4 || key == K_F5 || key == K_F6 ||
	    key == K_F7 || key == K_F8 || key == K_F9 || key == K_F10 || key == K_F11 || key == K_F12)
	    return CodeEditor::Key(key, count);
	
	if (key == 65553 || key == 1114129)
		return true;
	
	bool result = false;
	
	if (key != K_TAB) {
		result = CodeEditor::Key(key, count);
		
		if (key == '.') {
			p = GetColumnLine(GetCursor());
			String line = GetUtf8Line(p.y);
			line.Replace("\t", "    ");
			
			String wrd = "";
			int ofss = ::GetWord(line, wrd, p);
	
			//DUMP((char)line[]);
			anch.x = p.x - 1 - ofss;
			anch.y = p.y;
			
			Anchor = anch;
			
			WhenPopup();
			
			popUp.Clear();
			if (wrd.GetLength())
				Update(wrd);
			else {
				exactMatch = false;
				for (int i = 0; i < words.GetCount(); i++)
					popUp.Add(RawToValue(words[i]));
			}
			
			DoPopup(p);

			return true;
		}
	}
	
	p = GetColumnLine(GetCursor());
		
	line = GetUtf8Line(p.y);
	line.Replace("\t", "    ");
	
	ofss = ::GetWord(line, wrd, p);
	
	anch.x = p.x - 1 - ofss;
	anch.y = p.y;
	
	if (popUp.IsOpen() && anch != Anchor) {
		Anchor = anch;
		WhenPopup();
	}
	
	//result = CodeEditor::Key(key, count);
	/*if (p.x > 0 && (line[p.x - 1] == ')' || line[p.x - 1] == ';' || line[p.x - 1] == ']' || line[p.x - 1] == '}' || line[p.x - 1] == '.')) {
		if (key == K_TAB)
			result = CodeEditor::Key(key, count);
		//if (popUp.IsOpen())
		//	popUp.Close();
		return result;
	}*/
	
	popUp.Clear();
	if (wrd.GetLength())
		Update(wrd);
	else {
		exactMatch = false;
		for (int i = 0; i < words.GetCount(); i++)
			popUp.Add(RawToValue(words[i]));
		if (words.GetCount())
			popUp.SetCursor(0);
	}
	
	if (key == K_TAB && popUp.IsOpen() && popUp.GetCursor() != -1) {
		Feed(wrd, ofss);
		return true;
	}
	
	if (popUp.GetCount() > 0 && exactMatch == false)
		DoPopup(p);
	else {
		if (popUp.IsOpen())
			popUp.Close();
	}

	if (key == K_TAB)
		result = CodeEditor::Key(key, count);
	
	return result;
}

void SmartEditor::Feed(const String& wrd, int ofss) {
	ZItem item = popUp[popUp.GetCursor()].To<ZItem>();
		
	if (item.Name.GetLength()) {
		int start = GetCursor() - ofss;
		SetCursor(start);
		Remove(start, wrd.GetLength());
		Insert(start, item.Name);
		SetCursor(start + item.Name.GetLength());
		WhenAssistChange(this);
	}
	
	popUp.Close();
}

void SmartEditor::DoPopup(Point p) {
	if (popUp.GetCount() == 0)
		return;
	
	Rect r = GetLineScreenRect(GetCursorLine());
	int mm = min(popUp.GetCount(), 20);
	
	if (!popUp.IsOpen()) {
		popUp.SetRect(r.left + p.x * GetFont().GetAveWidth(), r.top + GetFont().GetLineHeight() * 2, 400, mm * popUp.GetItemHeight() + 5);
	
		ignoreFocus = true;
		popUp.PopUp(this);
		SetFocus();
		ignoreFocus = false;
	}
	else {
		Rect r = popUp.GetRect();
		r.SetSize(r.GetSize().cx, mm * popUp.GetItemHeight() + 5);
		popUp.SetRect(r);
	}
}

void SmartEditor::OnAutoSelect() {
	Point p = GetColumnLine(GetCursor());
	String line = GetUtf8Line(p.y);
	line.Replace("\t", "    ");
	
	String wrd = "";
	
	int ofss = ::GetWord(line, wrd, p);
		
	if (popUp.GetCursor() != -1)
		Feed(wrd, ofss);
}

void SmartEditor::LostFocus() {
	if (ignoreFocus == false && popUp.IsOpen())
		popUp.Close();
	CodeEditor::LostFocus();
}

