#include "gui/Window.h"
#include"./../../Canvas/src/canvas.h"
#pragma once




class MainWindow : public gui::Window
{
private:
protected:
    //MenuBar _mainMenuBar;
    //ToolBar _toolBar;
    Canvas _mainView;
public:
    MainWindow();
    ~MainWindow();


};




