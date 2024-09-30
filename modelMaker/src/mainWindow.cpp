#include "mainWindow.h"
#include "settingsDialog.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"


 


MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800)),
    _tabView(gui::TabHeader::Type::Dynamic, 1,10),
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


bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{

    menuBarActionIDs action = (menuBarActionIDs) aiDesc._actionItemID;
    subMenuIDs menuID = (subMenuIDs) aiDesc._firstSubmenuID;

    if(action == menuBarActionIDs::Settings){
        auto settings = new settingsDialog(this);
        settings->setResizable(false);
        settings->keepOnTopOfParent();
        settings->open();
        return true;
    }
    


    if(action == menuBarActionIDs::EmptyModel && menuID == subMenuIDs::subMenuNewGraphical){
        showStartScreen(false);
        _tabView.addView(new GraphicalEditorView, "New View", &guiEditorIcon);
    }


    if(action == menuBarActionIDs::EmptyModel && menuID == subMenuIDs::subMenuNewText){
        showStartScreen(false);
        _tabView.addView(new TextEditorView, "New View", &textEditorIcon);
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

