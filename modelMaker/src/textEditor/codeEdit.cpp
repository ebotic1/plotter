#include "codeEdit.h"
#include "../ToolBar.h"
#include "View.h"
#include "../globalEvents.h"


const std::regex CodeEdit::varPattern = std::regex(R"((^|[^A-Za-z_\.])([a-zA-Z_](?:[\w.]+?)?)(?:$|[^A-Za-z_\.]))");
const std::regex CodeEdit::attribPattern = std::regex(R"(([a-zA-Z_]+?)(?:\s+)?=)");
const std::regex CodeEdit::expPattern = std::regex(R"(\^\((.+?)\)|([a-zA-Z_.]+?))");

void CodeEdit::onPaste(){
	processText();
}

CodeEdit::CodeEdit(TextEditorView *parent) : gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send),
                                             parent(parent)
{


}



bool CodeEdit::onKeyPressed(const gui::Key& key)
{
	parent->modelChanged();


	if(key.isCtrlPressed() && (key.getChar() == '+' || key.getChar() == '-')){
		constexpr double sizeIncrease = 0.5;
		if(key.getChar() == '+')
			GlobalEvents::settingsVars.textSize += sizeIncrease;
		else
			GlobalEvents::settingsVars.textSize -= sizeIncrease;

		GlobalEvents::settingsVars.textSize = std::clamp(GlobalEvents::settingsVars.textSize, 5., 100.);
		setFontSize(GlobalEvents::settingsVars.textSize);

		return true;
	}

	gui::Range r;
	getSelection(r);
	text = getText();
	int pEnd = text.find('\n', r.location);
	int pStart = text.findFromRight('\n', r.location);
	pStart = (pStart == -1) ? 0 : pStart;
	pEnd = (pEnd == -1) ? text.length() : pEnd;
	processText(gui::Range(pStart, pEnd - pStart));

	return false;
}

bool CodeEdit::onKeyReleased(const gui::Key& key)
{
	int x = 3;
	return false;
}

void CodeEdit::processText(const gui::Range& r)
{
	gui::Range rangeFound;
	setColor(r, td::ColorID::SysText);
	const char *start = text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, (const td::UTF8 *) text.begin() + r.location + r.length, match, varPattern)){
		rangeFound.location = match[2].first - text.begin();
		rangeFound.length = match[2].length();
		td::String m(match[2].first, match[2].length());
	
		if(parent->vars.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorVariable);
		else if(parent->params.contains(m))
			setColor(rangeFound, GlobalEvents::settingsVars.colorParam);
		start = match.suffix().first;
	}


	start = text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, (const td::UTF8 *) text.begin() + r.location + r.length, match, attribPattern)){
		if(td::String m(match[1].first, match[1].length()); modelNode::attributeKeywords.contains(m)){
			rangeFound.location = match[1].first - text.begin();
			rangeFound.length = match[1].length();
			setColor(rangeFound, GlobalEvents::settingsVars.colorAttribute);
		}
		start = match.suffix().first;
	}


	start = text.begin() + r.location;
	while(std::regex_search((const td::UTF8 *)start, (const td::UTF8 *) text.begin() + r.location + r.length, match, expPattern)){
		if(match[1].matched){
			rangeFound.location = match[1].first - text.begin();
			rangeFound.length = match[1].length();
			setColor(rangeFound, td::ColorID::Transparent);
		}


		
		//setColor(rangeFound, GlobalEvents::settingsVars.colorVariable);
		start = match.suffix().first;
	}

}

void CodeEdit::processText()
{
	text = getText();
	processText(gui::Range(0, text.length()));
}
