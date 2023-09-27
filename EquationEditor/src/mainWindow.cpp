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

bool MainWindow::onClick(gui::FileDialog* pDlg, td::UINT4 dlgID)
{
    if (dlgID == 1) {
        if (!_mainView.loadFile(pDlg->getFileName()))
            showAlert("Error", "Cant load file");

        return true;
    }

    if (dlgID == 2) {
        td::String path = pDlg->getFileName();

        if(!_mainView.saveAs(path))
            showAlert("Error", "Cant save file");
        return true;
    }

    return false;
}

bool MainWindow::onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI)
{
    if (menuID == 1 && actionID == 4) {//open
        gui::OpenFileDialog *p = new gui::OpenFileDialog(this, "Open model", {"*.txt", "*.xml"}, "Open");
        p->openModal(1, this);
        return true;
    }

    if (menuID == 1 && actionID == 2) {//save
        if (!_mainView.save())
            showAlert("Error", "Cant save file");
        return true;
       
    }

    if (menuID == 1 && actionID == 3) { //save as
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Save as", {"*.txt"}, "Save as");
        p->openModal(2, this);
        return true;
    }

    if (menuID == 1 && actionID == 5) { //export to xml
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Exporting XML for modelSolver", { "*.xml" }, "Export");
        p->openModal(2, this);
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







