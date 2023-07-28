#include "gui/Window.h"
#include "View.h"
#include "MenuBar.h"
#include "ToolBar.h"
#pragma once




class MainWindow : public gui::Window
{
private:
protected:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    View _mainView;
public:
    MainWindow();

    virtual bool onClick(gui::FileDialog* pDlg, td::UINT4 dlgID) override;
    virtual bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI) override;
    virtual void onInitialAppearance() override;
    

    ~MainWindow();
};





