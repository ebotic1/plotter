#include "mainWindow.h"
#include "settingsDialog.h"
#include <gui/FileDialog.h>

#include "guiEditor/view.h"


MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800)),
    _tabView(gui::TabHeader::Type::Dynamic, 1,10)
{
    setTitle("Model Maker");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);

    setCentralView(&_tabView);
    //showStartScreen(true);
    
}

void MainWindow::onInitialAppearance(){
    //_mainMenuBar.enableModelMenu(false); // krah!?!?!?
}

void MainWindow::showStartScreen(bool show)
{
    return;
    if(show){
        if(getCentralView() == &startingView)
            return;
        //_mainMenuBar.enableModelMenu(false);
        setCentralView(&startingView);
    }else{
        if(getCentralView() == &_tabView)
            return;
        //_mainMenuBar.enableModelMenu(true);
        setCentralView(&_tabView);
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
        _tabView.addView(new GraphicalEditorView, "New View", new gui::Image(":start"));
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

