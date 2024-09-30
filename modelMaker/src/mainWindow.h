#include <gui/Window.h>
#include <gui/TabView.h>
#include <td/String.h>
#include <gui/ViewSwitcher.h>

#include "guiEditor/contextMenu.h"
#include "MenuBar.h"
#include "startingView.h"
#include "ToolBar.h"
#include "globalEvents.h"
#pragma once





class MainWindow : public gui::Window
{
private:
protected:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::TabView _tabView;
    gui::ViewSwitcher _switcherView;
    StartingView startingView;
    ContextMenus _contextMenu;

    gui::Image textEditorIcon, guiEditorIcon;
public:
    MainWindow();

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    void showStartScreen(bool show);

    ~MainWindow();
};




