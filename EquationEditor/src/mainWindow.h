#include "gui/Window.h"
#include "View.h"
#include "MenuBar.h"
#include "ToolBar.h"
#pragma once




class MainWindow : public gui::Window
{
private:
protected:
    MenuBar _mainMenuBar;
    ToolBar _toolBar;
    View _mainView;
public:
    MainWindow();
    ~MainWindow();


};





