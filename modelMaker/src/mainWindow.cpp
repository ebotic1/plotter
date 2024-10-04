#include "mainWindow.h"
#include "settingsDialog.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"

void MainWindow::simulate()
{
    if(_tabView.getNumberOfViews() == 0)
        return;

    simulate((ViewForTab*)_tabView.getCurrentView());
}

MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800)),
      _tabView(gui::TabHeader::Type::Dynamic, 5, 25),
      textEditorIcon(":txtIcon"),
      guiEditorIcon(":guiIcon"),
      _switcherView(2)
{
    setTitle("Model Maker");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);
    setContextMenus(&_contextMenu);

    _switcherView.addView(&startingView, true);
    setCentralView(&_switcherView);


    _tabView.onClosedView([this](int) {
        if (_tabView.getNumberOfViews() == 1)
            showStartScreen(true);
    });

    _switcherView.addView(&_tabView);
    
}

void MainWindow::showStartScreen(bool show)
{
    if(show){
        if(_switcherView.getCurrentViewPos() == 0)
            return;
        //_mainMenuBar.enableModelMenu(false);
        _switcherView.showView(0);

    }else{
        if (_switcherView.getCurrentViewPos() == 1)
            return;
        //_mainMenuBar.enableModelMenu(true);
        _switcherView.showView(1);
    }
}

void MainWindow::changeTabName(const td::String &name)
{
    //promijeniti naziv trenutnog taba
}

bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{
    int menuID = aiDesc._menuID;

    if(menuID == toolBarID){ //ako je izvor toolbar

        auto action = toolBarActionIDs(aiDesc._actionItemID);

        if(action == toolBarActionIDs::Simulate){
            simulate();
            return true;
        }

        return false;
    }

    //izvor mora biti menu bar ili poziv preko global events
    menuBarActionIDs action = (menuBarActionIDs) aiDesc._actionItemID;
    subMenuIDs subMenuID = (subMenuIDs) aiDesc._firstSubmenuID;

    if(action == menuBarActionIDs::Settings){
        auto settings = new settingsDialog(this);
        settings->setResizable(false);
        settings->keepOnTopOfParent();
        settings->open();
        return true;
    }
    


    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewGraphical){
        showStartScreen(false);
        _tabView.addView(new ViewForTab(new GraphicalEditorView), tr("newGraphTab"), &guiEditorIcon);
        return true;
    }


    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewText){
        showStartScreen(false);
        _tabView.addView(new ViewForTab(new TextEditorView), tr("newTextTab"), &textEditorIcon);
        return true;
    }

    if(action == menuBarActionIDs::Simulate){
        simulate();
        return true;
    }

    if(action == menuBarActionIDs::OpenFromFile && subMenuID == subMenuIDs::subMenuNewModel){

    }


    /*
    if (aiDesc._menuID == 1 && aiDesc._actionItemID == 4) {//open
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Open model", { {"Text", "*.txt"},{"XML model solver", "*.xml"} }, "Open");
        p->openModal([this](gui::FileDialog* pDlg) {
            if (!_mainView.loadFile(pDlg->getFileName()))
                showAlert("Error", "Cant load file");
            });
        return true;
    }

    if (aiDesc._menuID == 1 && aiDesc._actionItemID == 2) {//save
        if (!_mainView.save())
            showAlert("Error", "Cant save file");
        return true;
       
    }

    if (aiDesc._menuID == 1 && aiDesc._actionItemID == 3) { //save as
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Save as", {"*.txt"}, "Save as");
        p->openModal([this](gui::FileDialog* pDlg) {
            if (!_mainView.saveAs(pDlg->getFileName()))
                showAlert("Error", "Cant save file");
            });
        return true;
    }

    if (aiDesc._menuID == 1 && aiDesc._actionItemID == 5) { //export to xml
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Exporting XML for modelSolver", { "*.xml" }, "Export");
        p->openModal([this](gui::FileDialog* pDlg) {
            if (!_mainView.saveAs(pDlg->getFileName()))
                showAlert("Error", "Cant save file");
            });
        return true;
    }

*/
    return false;
}





MainWindow::~MainWindow(){}

