#pragma once
#include "gui/Application.h"
#include "gui/WinMain.h"
#include "mainWindow.h"

#include "gui/ContextMenu.h"
#include <gui/ContextMenus.h>


class contextMenuModel : public gui::ContextMenu{
private:
	gui::SubMenu sub;
public:
	contextMenuModel(): sub(100, "cm1", 2)
	{
		auto& items = sub.getItems();
		items[0].initAsActionItem("one", 10); 
		items[1].initAsActionItem("two", 20, "d");
		setItem(&sub);
	}
};


class ContextMenus : public gui::ContextMenus{
protected:
	contextMenuModel m1;
public:
	ContextMenus(): gui::ContextMenus(1){
		setItem(0, &m1);
	}
};

class application : public gui::Application {
	ContextMenus cMenu;

	gui::Window* createInitialWindow() override {
		
		return new mainWindow();
	}

public:
	application(int argc, const char** argv): gui::Application(argc, argv){
		setContextMenus(&cMenu);
	}

};

int main(int argcnt, const char** argv) {

	application app(argcnt, argv);
	app.init("BA");

	return app.run();


}