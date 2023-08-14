#pragma once
#include "gui/SplitterLayout.h"
#include "gui/PropertyEditorSwitcher.h"
#include "gui/View.h"
#include "canvas.h"
#include "gui/TextEdit.h"
#include "blockSettings.h"
#include "property.h"
//#include "globals.cpp"




class modelSettings : public gui::View {
	gui::VerticalLayout v;
	property name;
	

public:
	modelSettings() : v(1), name("model name", td::DataType::string8, "name of the model that will be generated") {
		v.appendLayout(name);
		setLayout(&v);

	}

};


class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
public:
	properties() {
		addView(&modSettings);
		//addView(&globals::block_properties);
		showView(0, true);
	}

};


class mainView : public gui::View {
	gui::SplitterLayout spliter;
	kanvas _canvas;

	properties props;


public:
	mainView(): spliter(gui::SplitterLayout::Orientation::Horizontal) {

		spliter.setContent(_canvas, props);		
		setLayout(&spliter);
	}

	void switchToView(int number) {
		props.showView(number, true);
	}

	bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI) {
		
		
		return true;
	}

	virtual bool onSpecialChar(gui::Object* pSender, char key) {

		return true;
	}


};





