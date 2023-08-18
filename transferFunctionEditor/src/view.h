#pragma once
#include "gui/SplitterLayout.h"
#include "gui/PropertyEditorSwitcher.h"
#include "gui/View.h"
#include "canvas.h"
#include "gui/TextEdit.h"
#include "blockSettings.h"
#include "property.h"
#include "globals.h"
#include "blockSettings.h"

blockSettings* globals::block_properties = nullptr;
gui::ViewSwitcher* globals::switcher = nullptr;



class modelSettings : public gui::View {
	gui::VerticalLayout v;
	elementProperty name;
	

public:
	modelSettings() : v(1), name("model name", td::DataType::string8, "name of the model that will be generated") {
		v.append(name);
		setLayout(&v);

	}

};


class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	blockSettings bSettings;
public:
	properties() {
		globals::switcher = this;
		globals::block_properties = &bSettings;
		addView(&modSettings);
		addView(globals::block_properties);
		showView(0, true);
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


};


void globals::refreshCanvas() {
	ptr->reDraw();
}





