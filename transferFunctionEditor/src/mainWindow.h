#include "gui/Window.h"
#include "view.h"

#include "gui/MenuBar.h"
class menuBar : public gui::MenuBar {
public:
	menuBar() : gui::MenuBar(0) {

	}
};



class mainWindow : public gui::Window {
protected:
mainView view;
menuBar menu;
public:
	mainWindow() : gui::Window(gui::Size(1000, 1000)) {
		setTitle("ovo ne radi nista??");
		menu.setAsMain(this);
		setCentralView(&view);
	}

	virtual bool onFinishEdit(gui::LineEdit* pCtrl) {

		return true;
	}

	bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI);

	bool onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu);

};

bool mainWindow::onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI) {

	return false;
}

bool mainWindow::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) {


	return true;
}


