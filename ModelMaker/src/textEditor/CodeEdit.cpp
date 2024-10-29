#include "CodeEdit.h"
#include "../ToolBar.h"
#include "View.h"
#include "../GlobalEvents.h"

//constexpr bool IZBACI_ENTER_FUNKCIONALNOST = true;

const std::regex CodeEdit::varPattern = std::regex(R"((?:^|[^A-Za-z_\.])(base\.)?([a-zA-Z_](?:[\w.]*?))(?:$|[^0-9A-Za-z_\.]))");
const std::regex CodeEdit::attribPattern = std::regex(R"((?:\[|,)\s*([a-zA-Z]+?)\s*?=)");
const std::regex CodeEdit::expPattern = std::regex(R"(\^(?:(\([^;\n]+\))|((?:[0-9]+?)?(?:[\w_.]+)?)))");
const std::regex CodeEdit::numberPattern = std::regex(R"((?:(e-[0-9][0-9\.]*)|(?:(?:^|[^\w])([0-9][0-9\.]*))))");

const std::regex CodeEdit::modelKeywordSearch = std::regex(R"((?:(Model)(?:[^\n]*?:)|(end)))");//unused




void CodeEdit::onPaste(){
	_parent->modelChanged();
	highlightSyntax();
}

void CodeEdit::syntaxHighlightingForCurrentLine()
{
	if (_disableGuardOnNextChange) {
		_disableGuardOnNextChange = false;
		_cursorChangeGuard = false;
		return;
	}
	if (_cursorChangeGuard) {
		return;
	}

	_text = getText();
	if (_text.isNull())
		_parent->setVersion(0);

	

	int pEnd = _text.find('\n', _pastRange.location + 1);
	int pStart = _text.findFromRight('\n', _pastRange.location - ((_pastRange.location == 0) ? 0 : 1));
	pStart = (pStart == -1) ? 0 : pStart;
	pEnd = (pEnd == -1) ? _text.length() : pEnd;
	highlightSyntax(gui::Range(pStart, pEnd - pStart));

	getSelection(_pastRange);
}





CodeEdit::CodeEdit(TextEditorView* parent):
	gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send),
    _parent(parent)
{

	//setFontSize(GlobalEvents::settingsVars.textSize);
	if (GlobalEvents::settingsVars.font.cCompare("Default") != 0)
		setFontName(GlobalEvents::settingsVars.font);

	onChangedSelection([this]() {syntaxHighlightingForCurrentLine(); });

}


bool CodeEdit::onKeyPressed(const gui::Key& key)
{
	_cursorChangeGuard = true;
	bool retVal = processKeyPress(key);
	if (key.getVirtual() == gui::Key::Virtual::Up || key.getVirtual() == gui::Key::Virtual::Down || key.getVirtual() == gui::Key::Virtual::Left || key.getVirtual() == gui::Key::Virtual::Right){
		_cursorChangeGuard = false;
		return retVal;
	}
	else if (retVal) {
		highlightSyntax(false);
	}

	getSelection(_pastRange);

	_disableGuardOnNextChange = true;

	return retVal;
}


inline bool CodeEdit::processKeyPress(const gui::Key& key)
{
	bool ctrlForZooom = false;
#ifdef MU_MACOS
	ctrlForZooom = key.isCmdPressed();
#else
	ctrlForZooom = key.isCtrlPressed();
#endif

	if (ctrlForZooom && key.isASCII())
	{
		char ch = key.getChar();
		if (ch == '+' || ch == '-')
		{
			constexpr double sizeIncrease = 0.5;
			if (key.getChar() == '+')
				GlobalEvents::settingsVars.textSize += sizeIncrease;
			else
				GlobalEvents::settingsVars.textSize -= sizeIncrease;

			GlobalEvents::settingsVars.textSize = std::clamp(GlobalEvents::settingsVars.textSize, 5., 100.);
			setFontSize(GlobalEvents::settingsVars.textSize);

			return true;
		}
	}


	if (key.getVirtual() != gui::Key::Virtual::F4 && key.getCode() != 65513/*alt*/ && key.getVirtual() != gui::Key::Virtual::Up && \
		key.getVirtual() != gui::Key::Virtual::Down && key.getVirtual() != gui::Key::Virtual::Left && key.getVirtual() != gui::Key::Virtual::Right) 
		//da se moze izaci na alt+f4 bez da se registruje update modela
	{
		_parent->modelChanged();
	}



	bool returnPressed = false;
	if (key.isVirtual())
	{
		auto ch = key.getVirtual();
		if (ch == gui::Key::Virtual::NumEnter)
		{
			returnPressed = true;
		}

	}
	else if (key.isASCII())
	{
		char ch = key.getChar();
		if (ch == '\r' || ch == '\n')
			returnPressed = true;
	}
	else {
		auto ch = key.getCode();
		if (ch == 65293) //enter
			returnPressed = true;
	}

	if (!returnPressed)
		return false;

	_text = getText();
	_cursorChangeGuard = true;

	if constexpr (/*returnPressed*/ true)
	{
		gui::Range r;
		getSelection(r);

		constexpr char spaceChar = '\t';
		auto pocetak = _text.begin() + r.location - 1;
		while (pocetak > _text.begin()) {
			if (*pocetak == ' ' || *pocetak == '\t' || *pocetak == ';' || *pocetak == '\n') {
				--pocetak;
				continue;
			}
			break;
		}

		bool addSpace = false;
		if (*pocetak == ':')
			addSpace = true;

		while (--pocetak != _text.begin()) {
			if (*pocetak == '\n')
				break;
		}

		int spaceCnt = (*pocetak == ' ' || *pocetak == '\t');
		while (++pocetak != _text.end()) {
			if (*pocetak == ' ' || *pocetak == '\t') {
				++spaceCnt;
				continue;
			}
			break;
		}

		spaceCnt = addSpace ? spaceCnt + 1 : spaceCnt;

		char* tabString = new char[spaceCnt + 2];
		std::fill(tabString + 1, tabString + 1 + spaceCnt, spaceChar);
		tabString[spaceCnt + 1] = '\0';
		tabString[0] = '\n';

		if (r.location == _text.length())
			appendString(tabString);
		else {
			cnt::StringBuilder str;
			str.appendString(_text.begin(), r.location);
			str << tabString;
			str.appendString(_text.c_str() + r.location, -1);
			str.getString(_text);
			setText(_text);
		}
		delete[](tabString);
		gui::Range cPos(r.location + 1 + spaceCnt, 0);
		setSelection(cPos, false);

		return true;
	}
	
	return false;
}




bool CodeEdit::onKeyReleased(const gui::Key& key)
{
	return false;
}

void CodeEdit::highlightSyntax(const gui::Range& r)
{
	gui::Range rangeFound;
	removeColor(r); //ovo se ne bi trebalo pozivati ali nekad se desavaju prolblemi bez ovoga
	setColor(r, GlobalEvents::settingsVars.colorText);
	//removeMarkup(r, gui::Font::Markup::Superscript);

	const td::UTF8* end = _text.begin() + r.location + r.length;
	const char* start = _text.begin() + r.location;


	while(std::regex_search((const td::UTF8 *)start, end, match, varPattern)){
		rangeFound.location = match[2].first - _text.begin();
		rangeFound.length = match[2].length();
		td::String m(match[2].first, match[2].length());
	
		if(_parent->getVars().contains(m) || _parent->_varsNotRoot.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorVariable);
		else if(_parent->getParams().contains(m) || _parent->_paramsNotRoot.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorParam);
		else if (modelNode::functionKeywords.contains(m) || _parent->_expressions.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorFunctions);
		else if (modelNode::constantsKeywords.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorConstants);
		else if (modelNode::syntaxKeywords.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorKeyword);

		if(match[1].matched){
			rangeFound.location = match[1].first - _text.begin();
			rangeFound.length = 5; //base.
			setColor(rangeFound, GlobalEvents::settingsVars.colorKeyword);
		}
			
		
		start = match.suffix().first;
	}

	start = _text.begin() + r.location;
	while (std::regex_search((const td::UTF8*)start, end, match, numberPattern)) {
		if(match[1].matched){
			rangeFound.location = match[1].first - _text.begin();
			rangeFound.length = match[1].length();
		}else{
			rangeFound.location = match[2].first - _text.begin();
			rangeFound.length = match[2].length();
		}
		setColor(rangeFound, GlobalEvents::settingsVars.colorConstants);
		start = match.suffix().first;
	}

	start = _text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, end, match, attribPattern)){
		if(td::String m(match[1].first, match[1].length()); modelNode::attributeKeywords.contains(m)){
			rangeFound.location = match[1].first - _text.begin();
			rangeFound.length = match[1].length();
			setColor(rangeFound, GlobalEvents::settingsVars.colorAttribute);
		}
		start = match.suffix().first;
	}

	/* for exponent marking
	start = _text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, end, match, expPattern)){
		if(match[1].matched){
			int openBrackets = 0;
			const td::UTF8 *kraj = match[1].first + match[1].length();
			const td::UTF8 *pocetak = match[1].first;
			while (++pocetak != end) {
				if (*pocetak == '(')
					++openBrackets;
				else if ((*pocetak == ')') && (openBrackets-- == 0)) {
					kraj = pocetak+1;
					break;
				}
			}

			rangeFound.location = match[1].first - _text.begin();
			if (openBrackets == -1)
				rangeFound.length = kraj - match[1].first;
			else
				rangeFound.length = 0;
		}
		else{
			rangeFound.location = match[2].first - _text.begin();
			rangeFound.length = match[2].length();
		}


		setMarkup(rangeFound, gui::Font::Markup::Superscript);
		start = match.suffix().first;
	}
	*/

	
	char pattern[3];
	for (int i = 0; i < 2; ++i) {
		int commentStart = r.location;
		if (i == 0) {
			pattern[0] = '/';
			pattern[1] = '/';
			pattern[2] = '\0';
		}
		else {
			pattern[0] = '#';
			pattern[1] = '\0';
		}

		while (commentStart = _text.find(pattern, commentStart), commentStart != -1) {
			int commentEnd = _text.find("\n", commentStart);
			commentEnd = (commentEnd == -1) ? _text.length() : commentEnd;
			setColor(gui::Range(commentStart, commentEnd - commentStart), GlobalEvents::settingsVars.colorComment);
			removeMarkup(gui::Range(commentStart, commentEnd - commentStart), gui::Font::Markup::Superscript);
			commentStart += 2;
		}
	}

}

void CodeEdit::highlightSyntax(bool getTextFromEdit)
{
	if(getTextFromEdit)
		_text = getText();
	highlightSyntax(gui::Range(0, _text.length()));
}
