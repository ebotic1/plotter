#include "CodeEdit.h"
#include "../ToolBar.h"
#include "View.h"
#include "../GlobalEvents.h"


const std::regex CodeEdit::varPattern = std::regex(R"((^|[^A-Za-z_\.])([a-zA-Z_](?:[\w.]+?)?)(?:$|[^0-9A-Za-z_\.]))");
const std::regex CodeEdit::attribPattern = std::regex(R"(([a-zA-Z_]+?)(?:\s+)?=)");
const std::regex CodeEdit::expPattern = std::regex(R"(\^(?:(\([^;\n]+\))|((?:[0-9]+?)?(?:[\w_.]+)?)))");

void CodeEdit::onPaste(){
	parent->modelChanged();
	processText();
}

CodeEdit::CodeEdit(ViewForTab::BaseClass *parent) : 
	gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send),
    parent(parent)
{

	//setFontSize(GlobalEvents::settingsVars.textSize);
	if (GlobalEvents::settingsVars.font.cCompare("Default") != 0)
		setFontName(GlobalEvents::settingsVars.font);

}


bool CodeEdit::onKeyPressed(const gui::Key& key)
{
	parent->modelChanged();

    bool ctrlForZooom = false;
#ifdef MU_MACOS
    ctrlForZooom = key.isCmdPressed();
#else
    ctrlForZooom = key.isCtrlPressed();
#endif
    
    if (ctrlForZooom && key.isASCII())
    {
        char ch = key.getChar();
        if(ch == '+' || ch == '-')
        {
            constexpr double sizeIncrease = 0.5;
            if(key.getChar() == '+')
                GlobalEvents::settingsVars.textSize += sizeIncrease;
            else
                GlobalEvents::settingsVars.textSize -= sizeIncrease;

            GlobalEvents::settingsVars.textSize = std::clamp(GlobalEvents::settingsVars.textSize, 5., 100.);
            setFontSize(GlobalEvents::settingsVars.textSize);

            return true;
        }
    }


	gui::Range r;
	getSelection(r);
	text = getText();
    //appendString("XX");
    //setSelection(r, false);
    return false;  //IDz: Ovo sam dodao da mogu editovati text. Kad uklonim ovo stalno skace na zadnju poziciju


	int pEnd = text.find('\n', r.location + 1);
	int pStart = text.findFromRight('\n', r.location - ((r.location == 0) ? 0 : 1));
	pStart = (pStart == -1) ? 0 : pStart;
	pEnd = (pEnd == -1) ? text.length() : pEnd;
	processText(gui::Range(pStart, pEnd - pStart));


    bool returnPressed = false;
    if (key.isASCII())
    {
        char ch = key.getChar();
        if (ch == '\r' || ch == '\n')
            returnPressed = true;
    }
    else
    {
        if (key.isVirtual())
        {
            auto ch = key.getVirtual();
            if (ch == gui::Key::Virtual::NumEnter)
            {
                returnPressed = true;
            }
        }
    }
	if (returnPressed)
    {
		constexpr char spaceChar = '\t';
		auto pocetak = text.begin() + r.location-1;
		while (pocetak > text.begin()) {
			if (*pocetak == ' ' || *pocetak == '\t' || *pocetak == ';' || *pocetak == '\n') {
				--pocetak;
				continue;
			}
			break;
		}

		bool addSpace = false;
		if (*pocetak == ':')
			addSpace = true;

		while (--pocetak != text.begin()) {
			if (*pocetak == '\n' || *pocetak == ';')
				break;
		}

		int spaceCnt = (*pocetak == ' ' || *pocetak == '\t');
		while (++pocetak != text.end()) {
			if (*pocetak == ' ' || *pocetak == '\t') {
				++spaceCnt;
				continue;
			}
			break;
		}

		spaceCnt = addSpace ? spaceCnt + 1 : spaceCnt;

		char* tabString = new char[spaceCnt + 2];
		std::fill(tabString + 1, tabString + 1 + spaceCnt, spaceChar);
		tabString[spaceCnt+1] = '\0';
		tabString[0] = '\n';

		if(r.location == text.length())
			appendString(tabString);
		else {
			/* // kurzor se pomjeri na kraj tako da ne valja
			cnt::StringBuilder str;
			str.appendString(text.begin(), r.location);
			str << tabString;
			str.appendString(text.c_str() + r.location, -1);
			str.getString(text);
			setText(text);
			*/
			return false;
		}
		delete[](tabString);
		return true;
	}

	return false;
}

bool CodeEdit::onKeyReleased(const gui::Key& key)
{
	return false;
}

void CodeEdit::processText(const gui::Range& r)
{
	gui::Range rangeFound;
	removeColor(r);
	removeMarkup(r, gui::Font::Markup::Superscript);

	const td::UTF8* end = text.begin() + r.location + r.length;

	const char *start = text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, end, match, varPattern)){
		rangeFound.location = match[2].first - text.begin();
		rangeFound.length = match[2].length();
		td::String m(match[2].first, match[2].length());
	
		if(parent->getVars().contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorVariable);
		else if(parent->getParams().contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorParam);
		start = match.suffix().first;
	}


	start = text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, end, match, attribPattern)){
		if(td::String m(match[1].first, match[1].length()); modelNode::attributeKeywords.contains(m)){
			rangeFound.location = match[1].first - text.begin();
			rangeFound.length = match[1].length();
			setColor(rangeFound, GlobalEvents::settingsVars.colorAttribute);
		}
		start = match.suffix().first;
	}


	start = text.begin() + r.location;
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

			rangeFound.location = match[1].first - text.begin();
			if (openBrackets == -1)
				rangeFound.length = kraj - match[1].first;
			else
				rangeFound.length = 0;
		}
		else{
			rangeFound.location = match[2].first - text.begin();
			rangeFound.length = match[2].length();
		}


		setMarkup(rangeFound, gui::Font::Markup::Superscript);
		start = match.suffix().first;
	}


	int commentStart = r.location;
	while(commentStart = text.find("//", commentStart), commentStart != -1){
		int commentEnd = text.find("\n", commentStart);
		commentEnd = (commentEnd == -1) ? text.length() : commentEnd;
		setColor(gui::Range(commentStart, commentEnd - commentStart), GlobalEvents::settingsVars.colorComment);
		removeMarkup(gui::Range(commentStart, commentEnd - commentStart), gui::Font::Markup::Superscript);
		commentStart += 2;
	}

}

void CodeEdit::processText()
{
	text = getText();
	processText(gui::Range(0, text.length()));
}
