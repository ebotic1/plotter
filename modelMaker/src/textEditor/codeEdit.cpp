#include "codeEdit.h"
#include "../ToolBar.h"
#include "View.h"



CodeEdit::CodeEdit(TextEditorView *parent) : 
gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send),
parent(parent)
{

	

}



bool CodeEdit::onKeyPressed(const gui::Key& key)
{
	parent->modelChanged();

	gui::Range r;
	getSelection(r);
	text = getText();
	int pEnd = text.find('\n', r.location);
	int pStart = text.findFromRight('\n', r.location);
	pStart = (pStart == -1) ? 0 : pStart;
	pEnd = (pEnd == -1) ? text.length() - 1 : pEnd;
	processText(gui::Range(pStart, pEnd));

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
	while(std::regex_search((const td::UTF8 *)text.begin() + r.location, (const td::UTF8 *) text.begin() + r.location + r.length, match, modelNode::varPatten)){
		rangeFound.location = match[1].first - text.begin();
		rangeFound.length = match[1].length();
		setColor(r, td::ColorID::Crimson);
	}

}

