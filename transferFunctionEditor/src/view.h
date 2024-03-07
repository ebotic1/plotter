#pragma once
#include "gui/SplitterLayout.h"
#include "gui/View.h"
#include "canvas.h"
#include "gui/ViewScroller.h"
#include "propertySwitcher.h"


class mainView : public gui::View {
	gui::SplitterLayout spliter;
	properties props;

	gui::ViewScroller scroller;
	
	kanvas _canvas;

public:
	mainView();
	void switchToView(int number) {
		props.showView(number, true);
	}

	gui::Consumer* getCanvas() {
		return &_canvas;
	}


};







