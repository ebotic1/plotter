#include "gui/Window.h"
#include "view.h"
#include "gui/FileDialog.h"

#include "gui/MenuBar.h"
class menuBar : public gui::MenuBar {
	gui::SubMenu modelMenu;
public:
	menuBar() : gui::MenuBar(1), modelMenu(1, "Model", 5) {
		auto &it = modelMenu.getItems();
		it[0].initAsActionItem("New", 1);
		it[1].initAsActionItem("Open", 2);
		it[2].initAsActionItem("Save", 3);
		it[3].initAsActionItem("Save as", 4);
		it[4].initAsActionItem("Export to XML", 100);


		auto &items = this->getItems();
		items[0] = &modelMenu;
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
		//menu.forwardMessagesTo(view.getCanvas());
		setCentralView(&view);
	}

	virtual bool onFinishEdit(gui::LineEdit* pCtrl) {

		return true;
	}


	bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
	//virtual bool onClick(gui::FileDialog* pDlg, td::UINT4 dlgID);
	bool onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) override;

};


bool mainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc) {


	return false;
}


bool mainWindow::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) {


	return true;
}

