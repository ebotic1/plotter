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
	contextMenuModel(): sub(100, "cm1", 3)
	{
		auto& items = sub.getItems();
		items[0].initAsActionItem("new tf block", 10);
		items[1].initAsActionItem("new sum block", 11);
		items[2].initAsActionItem("new nonlinear block", 12);

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