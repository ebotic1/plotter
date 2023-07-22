#include "gui/Window.h"
#include "mainWindow.h"





MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800))
{
    setTitle("Equation Editor");

    setCentralView(&_mainView);

}


MainWindow::~MainWindow(){}







