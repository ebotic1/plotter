#include "gui/Window.h"
#include "mainWindow.h"
#include "gui/FileDialog.h"





MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800))
{
    setTitle("Equation Editor");
    _mainMenuBar.setAsMain(this);
    //setToolBar(_toolBar);
    setCentralView(&_mainView);

}

bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{
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

    return false;
}

void MainWindow::onInitialAppearance(){
    const char* homePath = getenv("HOME");

    if (homePath == nullptr) 
        return;
    td::String path = homePath;
    path += "\\modelSolver\\test.txt";
    _mainView.loadFile(path);
}




MainWindow::~MainWindow(){}







