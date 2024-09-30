#include "codeEdit.h"
#include <regex>


CodeEdit::CodeEdit() :
	gui::TextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send)
{
	

}



bool CodeEdit::onKeyPressed(const gui::Key& key)
{
	gui::Range r;
	getSelection(r);
	const auto& text = getText();
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
	const auto& text = getText();
	//const char *str = text.subStr(r.location, r.length).c_str();
	
	

}


