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

    _mainView.addFunction(x, y, broj);

    delete[] x;
    delete[] y;

    setCentralView(&_mainView);


}


MainWindow::~MainWindow(){}







