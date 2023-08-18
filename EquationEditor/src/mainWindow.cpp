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
        _mainView.loadXML(pDlg->getFileName());
        //delete pDlg; crash aplikacije ?
    }

    if (dlgID == 2) {
        td::String path = pDlg->getFileName();
        if (path.subStr(path.length() - 5, path.length() - 1).cCompare(".xml") == 0)
            _mainView.saveAsXML(path);
        else {
            path += "model.xml";
            _mainView.saveAsXML(path);
        }
            
        
    }

    return false;
}

bool MainWindow::onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI)
{
    if (menuID == 1 && actionID == 4) {//open
        gui::OpenFileDialog *p = new gui::OpenFileDialog(this, "Open model", { "*.xml" }, "Open");               
        p->openModal(1, this);
        return true;
    }

    if (menuID == 1 && actionID == 2) {//save
        _mainView.saveXML();
        return true;
    }

    if (menuID == 1 && actionID == 3) { //save as
        gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Save model", { "*.xml" /*nekako omoguciti otvaranje foldera*/}, "Save as");
        p->openModal(2, this);
        return true;
    }

    return false;
}

void MainWindow::onInitialAppearance(){
    _mainView.loadXML("C:\\Users\\eb\\modelSolver\\test.xml");
}




MainWindow::~MainWindow(){}







