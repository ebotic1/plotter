#pragma once
#include "gui/Label.h"
#include "../../../Canvas/inc/canvas.h"
#include "gui/SplitterLayout.h"
#include "../baseView.h"
#include "codeEdit.h"
#include "logView.h"
#include "../../../EquationToXML/inc/nodes.h"


class EquationView : public gui::View {

	gui::TextEdit textCodeNumbers;
	gui::HorizontalLayout _hl;

public:
	CodeEdit textMain;

	EquationView();

};


class TextEditorView : public BaseViewForTab {

	gui::SplitterLayout splitter;

	EquationView equationView;
	LogView logView;

	void getModelNode(modelNode &model);


public:

	TextEditorView();
	void simulate(double startTime, double endTime, double stepTime);
	
};