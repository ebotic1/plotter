#pragma once
#include <gui/TextEdit.h>
#include <gui/HorizontalLayout.h>
#include <regex>
#include "../baseView.h"

class TextEditorView;

class CodeEdit : public gui::TextEdit {
	ViewForTab::BaseClass *parent;
	td::String text;
	std::cmatch match;
	void onPaste();
	static const std::regex varPattern;
	static const std::regex attribPattern;
	static const std::regex expPattern;


public:

	CodeEdit(ViewForTab::BaseClass *parent);
	bool onKeyPressed(const gui::Key& key) override;
	bool onKeyReleased(const gui::Key& key) override;
	void processText(const gui::Range& r);
	void processText();

};


