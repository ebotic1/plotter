#pragma once
#include "gui/Label.h"
#include "../../../Canvas/inc/canvas.h"
#include "gui/SplitterLayout.h"
#include "../baseView.h"
#include "codeEdit.h"
#include "logView.h"


class EquationView : public gui::View {

	gui::TextEdit textCodeNumbers;
	CodeEdit textMain;
	gui::HorizontalLayout _hl;

public:

	EquationView() :
		_hl(1)
	{
		_hl << textMain;
		setLayout(&_hl);
	}

};


class TextEditorView : public BaseViewForTab {

	gui::SplitterLayout splitter;

	EquationView equationView;
	LogView logView;

public:

	TextEditorView();
	
};