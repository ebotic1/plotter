#pragma once
#include "gui/MenuBar.h"
#include "gui/MenuItem.h"


class MenuBar : public gui::MenuBar {
	gui::SubMenu model;
public:
	MenuBar() : gui::MenuBar(1), model(1, "Model", 4)
	{
		auto &items = model.getItems();
		items[0].initAsActionItem("New", 1, "n");//submenu
		items[1].initAsActionItem("Save", 2, "s");
		items[2].initAsActionItem("Save as", 3, "sa");
		items[3].initAsActionItem("Open", 4, "o");
	
		_menus[0] = &model;
	
		
	}

	~MenuBar(){}
};