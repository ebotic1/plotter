#pragma once
#include "codeEdit.h"
#include "../../../EquationToXML/inc/nodes.h"
#include <gui/HorizontalLayout.h>
#include "../baseView.h"


class TextEditorView;

class EquationView : public gui::View {

	gui::TextEdit textCodeNumbers;
	gui::HorizontalLayout _hl;


public:
	CodeEdit textMain;

	EquationView();

};


class TextEditorView : public ViewForTab::BaseClass {

	EquationView equationView;
	gui::HorizontalLayout _hl;

public:

	

	TextEditorView();
	void getModel(modelNode& model) override;

};