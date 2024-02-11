#pragma once
#include "gui/SplitterLayout.h"
#include "gui/View.h"
#include "canvas.h"
#include "gui/TextEdit.h"
#include "blockSettings.h"
#include "./../../common/property.h"
#include "globals.h"
#include "blockSettings.h"
#include "modelSettings.h"
#include <gui/ViewScroller.h>

blockSettings* globals::block_properties = nullptr;
gui::ViewSwitcher* globals::switcher = nullptr;
modelSettings *globals::model_settings = nullptr;




class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	blockSettings bSettings;

	gui::VerticalLayout vl;
	gui::LineEdit edit;
	gui::View pogled;

public:
	properties(): vl(1) {

		vl << edit;
		pogled.setLayout(&vl);

		globals::switcher = this;
		globals::block_properties = &bSettings;
		addView(&pogled, true);
		//addView(&bSettings);
		

		//modSettings.edit.hide(false, false);
		
	}


};

kanvas* ptr;


class mainView : public gui::View {
	gui::SplitterLayout spliter;
	properties props;

	gui::ViewScroller scroller;
	
	kanvas _canvas;

public:
	mainView(): spliter(gui::SplitterLayout::Orientation::Horizontal), scroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide){
		ptr = &_canvas;
		scroller.setContentView(&_canvas);
		spliter.setContent(scroller, props);		
		
		setLayout(&spliter);

		
		
	}

	void switchToView(int number) {
		props.showView(number, true);
	}

	gui::Consumer* getCanvas() {
		return &_canvas;
	}


};



void globals::refreshCanvas() {
	ptr->reDraw();
}





