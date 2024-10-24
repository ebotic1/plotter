#pragma once
#include <gui/SplitterLayout.h>
#include <gui/Window.h>
#include <gui/TabView.h>
#include <td/String.h>
#include <gui/ViewSwitcher.h>

#include "guiEditor/contextMenu.h"
#include "MenuBar.h"
#include "StartingView.h"
#include "ToolBar.h"
#include "../../EquationToXML/inc/nodes.h"
#include "BaseView.h"
#include "DataDrawer.h"

class ViewForTab;


struct fileModels{
    td::String path;
    modelNode model;
    std::chrono::nanoseconds timeModified;
    fileModels(const td::String &path):
        timeModified(0),
        path(path)
        {}
};

class DataDrawerWindow : public gui::Window {
    DataDraw* storedView;
public:
    static constexpr int dataDrawerWindowID = 10;
    DataDrawerWindow(gui::Window* parent, DataDraw *mainView);
    ~DataDrawerWindow();
};

class MainWindow : public gui::Window
{
private:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    gui::TabView _tabView;
    gui::ViewSwitcher _switcherView;
    DataDraw dataDrawer;
    bool plotEmbedded = false;

    StartingView startingView;
    ContextMenus _contextMenu;

    gui::Image textEditorIcon, guiEditorIcon;

    static std::vector<fileModels> loadedModels;

    void simulate();
    void addTab(ViewForTab::BaseClass *tab, const td::String &settingsStr, const td::String &path = td::String());

public:
    MainWindow();
    struct exceptionCantAccessFile { td::String message; };
    struct exceptionCantFindTab { td::String message; };
    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
    void showStartScreen(bool show);
    void changeTabName(const td::String &name, ViewForTab *tab);
    int simulate(ViewForTab* tab);
    void openFile(const td::String& path);
    void onInitialAppearance() override;
    const modelNode &getModelFromTabOrFile(const td::String &modelNameOrPath);
    DataDraw* getDataDrawer(bool openWindow = true);

    bool shouldClose() override;
    ~MainWindow();
};




