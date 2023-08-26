#include "gui/Window.h"
#include "./../../Canvas/inc/canvas.h"
#include "gui/MenuBar.h"
#include "splitter.h"


class MenuBar : public gui::MenuBar {
    gui::SubMenu open, save;
public: 
    MenuBar() : gui::MenuBar(2), open(1, "Open", 2), save(2, "Export", 5) {

        auto &items = open.getItems();
        items[0].initAsActionItem("Open function", 1);
        items[1].initAsActionItem("Append function", 2);

        auto& items2 = save.getItems();
        items2[0].initAsActionItem("EPS", 3);
        items2[1].initAsActionItem("PDF", 4);
        items2[2].initAsActionItem("SVG", 5);
        items2[3].initAsActionItem("txt", 10);
        items2[4].initAsActionItem("xml", 11);

        auto& items3 = getItems();

        items3[0] = &open;
        items3[1] = &save;


    }

};


class MainWindow : public gui::Window
{
private:
protected:
    MenuBar _mainMenuBar;
    //ToolBar _toolBar;
    graph _graph;
    splitterLayout splitter;


public:
    MainWindow();
    ~MainWindow();

    bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI) override;
    bool onClick(gui::FileDialog* pDlg, td::UINT4 dlgID) override;


};




