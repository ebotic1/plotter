#pragma once
#include "gui/MenuBar.h"
#include "gui/MenuItem.h"

class smece : public gui::MenuItem {

public:
	smece() : gui::MenuItem() {
		this->initAsQuitAppActionItem("quit");
		
	}
};

class MenuBar : public gui::MenuBar {
	smece d;
public:
	MenuBar() : gui::MenuBar(1)
	{
		_menus[0] = &d;
	
	}

	~MenuBar(){}
};