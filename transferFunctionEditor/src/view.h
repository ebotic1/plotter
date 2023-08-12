#pragma once
#include "gui/SplitterLayout.h"
#include "gui/PropertyEditorSwitcher.h"
#include "gui/View.h"
#include "canvas.h"




class mainView : public gui::View {
	gui::SplitterLayout spliter;
	//kanvas _canvas;

	gui::HorizontalLayout h;


public:
	mainView(): spliter(gui::SplitterLayout::Orientation::Horizontal), h(1) {

		//spliter.setContent(_canvas, *new gui::View());
		//spliter.forwardMessagesTo(&_canvas);
		//h.append(_canvas);
		//setLayout(&h);
	}
};

