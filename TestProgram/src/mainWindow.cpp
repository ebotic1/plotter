#include "gui/Window.h"
#include "mainWindow.h"
#include "./../../Canvas/inc/canvas.h"



MainWindow::MainWindow()
    : gui::Window(gui::Geometry(600, 100, 1500, 1500))
{
    setTitle("Graph");

    int broj = 1001;
    gui::CoordType* x = new gui::CoordType[broj];
    gui::CoordType* y = new gui::CoordType[broj];

    for (int i = 0; i < broj; ++i) {
        x[i] = i;
        y[i] = i * i;
    }

    //_mainView.addFunction(x, y, broj);

    gui::CoordType* xx = new gui::CoordType[3];
    gui::CoordType* yy = new gui::CoordType[3];

    xx[0] = 100; xx[1] = 100; xx[2] = 300;
    yy[0] = 300; yy[1] = -1400; yy[2] = 300;

    _mainView.addFunction(xx, yy, 3);

    delete[] x;
    delete[] y;

    setCentralView(&_mainView);

    //_mainView.ZoomToWindow(gui::Geometry());

}


MainWindow::~MainWindow(){}







