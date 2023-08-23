#include "gui/Window.h"
#include "mainWindow.h"
#include "./../../Canvas/inc/canvas.h"



MainWindow::MainWindow()
    : gui::Window(gui::Geometry(600, 100, 1500, 1500))
{
    setTitle("Graph");

    _mainView.showMargins(2);

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

    _mainView.addFunction(xx, yy, 3);
    
    _mainView.addFunction(x, y, broj);


    

    delete[] x;
    delete[] y;

    setCentralView(&_mainView);

    


}


MainWindow::~MainWindow(){}







