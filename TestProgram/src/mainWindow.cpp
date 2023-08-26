#include "gui/Window.h"
#include "mainWindow.h"
#include "./../../Canvas/inc/canvas.h"

#include "gui/FileDialog.h"


MainWindow::MainWindow()
    : gui::Window(gui::Geometry(600, 100, 1500, 1500)), _graph(true, true, td::ColorID::Black), splitter(_graph)
{
    setTitle("Graph");
    _mainMenuBar.setAsMain(this);

    _graph.showMargins(2);

    int broj = 1001;
    gui::CoordType* x = new gui::CoordType[broj];
    gui::CoordType* y = new gui::CoordType[broj];

    for (int i = 0; i < broj; ++i) {
        x[i] = i;
        y[i] = i * i;
    }


    gui::CoordType* xx = new gui::CoordType[3];
    gui::CoordType* yy = new gui::CoordType[3];

    xx[0] = 0; xx[1] = 10; xx[2] = 100;
    yy[0] = 0; yy[1] = 10; yy[2] = 100;

    _graph.addFunction(xx, yy, 3);
    _graph.addFunction(x, y, broj);

  
    setCentralView(&splitter);

    
    splitter.refreshPicks();


}

bool resetGraph = true;

bool MainWindow::onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI){

    if (menuID == 1) {
        if (actionID > 2)
            return false;
        if (actionID == 1) {
            _graph.reset();
            resetGraph = true;
        }
        else {
            resetGraph = false;
        }

        auto f = new gui::FileDialog(this, "Read data", { "*.txt", "*.xml" }, "Open");
        f->openModal(1, this);

    }
    
    if (menuID == 2) {

        if (actionID == 11) {
            
            auto a = new gui::SaveFileDialog(this, "export data to xml", "*.xml");
            a->openModal(11, this);
            return true;
        }

        if (actionID == 10) {

            auto a = new gui::SaveFileDialog(this, "export data to txt", "*.txt");
            a->openModal(10, this);
            return true;
        }

    }

    return false;
}

bool MainWindow::onClick(gui::FileDialog* pDlg, td::UINT4 dlgID){
    if (dlgID == 1) {
        if (pDlg->getFileName().endsWithCI(".txt", 4))
            _graph.readTXT(pDlg->getFileName());
        if (pDlg->getFileName().endsWithCI(".xml", 4))
            _graph.readXML(pDlg->getFileName(), resetGraph);
        splitter.refreshPicks();
        return true;
    }

    if (dlgID == 11) {
        _graph.saveXML(pDlg->getFileName());
        return true;
    }

    if (dlgID == 10) {
        _graph.saveTXT(pDlg->getFileName());
        return true;
    }


    return false;
}
MainWindow::~MainWindow(){}







