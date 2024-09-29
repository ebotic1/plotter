#include <gui/Window.h>
#include <gui/TabView.h>
#include <td/String.h>

#include "guiEditor/contextMenu.h"
#include "MenuBar.h"
#include "startingView.h"
#include "ToolBar.h"
#pragma once



class MainWindow : public gui::Window
{
private:
protected:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::TabView _tabView;
    StartingView startingView;
    ContextMenus _contextMenu;

    gui::Image textEditorIcon, guiEditorIcon;
public:
    MainWindow();

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    virtual void onInitialAppearance() override;
    void showStartScreen(bool show);

    ~MainWindow();
};





