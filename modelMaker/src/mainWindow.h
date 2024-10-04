#pragma once
#include <gui/Window.h>
#include <gui/TabView.h>
#include <td/String.h>
#include <gui/ViewSwitcher.h>

#include "guiEditor/contextMenu.h"
#include "MenuBar.h"
#include "startingView.h"
#include "ToolBar.h"
#include "../../EquationToXML/inc/nodes.h"

class ViewForTab;

class MainWindow : public gui::Window
{
private:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::TabView _tabView;
    gui::ViewSwitcher _switcherView;
    StartingView startingView;
    ContextMenus _contextMenu;

    gui::Image textEditorIcon, guiEditorIcon;

    void simulate();

public:
    MainWindow();

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    void showStartScreen(bool show);
    void changeTabName(const td::String &name);
    void simulate(ViewForTab* tab);
    bool getModelFromFile(modelNode &model);

    ~MainWindow();
};




