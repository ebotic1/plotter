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
        if (!_mainView.loadXML(pDlg->getFileName()))
            showAlert("Error", "Cant load file");

            //delete pDlg; crash aplikacije ?
        return true;
    }

    if (dlgID == 2) {
        td::String path = pDlg->getFileName();

        if(!_mainView.saveAsXML(path))
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
        if (!_mainView.saveXML())
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
    _mainView.loadXML("C:\\Users\\eb\\modelSolver\\test.txt");
}




MainWindow::~MainWindow(){}







