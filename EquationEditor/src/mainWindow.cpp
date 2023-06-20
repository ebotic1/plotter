#include "gui/Window.h"
#include "mainWindow.h"





MainWindow::MainWindow()
    : gui::Window(gui::Geometry(600, 100, 1500, 1500))
{
    setTitle("Equation Editor");

    setCentralView(&_mainView);

}


MainWindow::~MainWindow(){}







