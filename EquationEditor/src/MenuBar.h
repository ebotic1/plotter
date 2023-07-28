#pragma once
#include "gui/MenuBar.h"
#include "gui/MenuItem.h"


class MenuBar : public gui::MenuBar {
	gui::SubMenu model;
public:
	MenuBar() : gui::MenuBar(1), model(1, "Model", 3)
	{
		auto &items = model.getItems();
		items[0].initAsActionItem("Save", 1, "s");
		items[1].initAsActionItem("Open", 2, "o");
		items[2].initAsActionItem("New", 3, "n");
	
		_menus[0] = &model;
	
		
	}

	~MenuBar(){}
};