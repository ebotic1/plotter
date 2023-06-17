#include "gui/Window.h"
#include "mainWindow.h"





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

    _mainView.dodajFunkciju(x, y, broj);
    _mainView.zoomToFit();

    std::vector<gui::CoordType> x_vec;
    std::vector<gui::CoordType> y_vec;

    for (int i = 0; i < broj; i++) {
        x_vec.push_back(i);
        y_vec.push_back(i*1000);
    }

    _mainView.dodajFunkciju(std::move(x_vec), std::move(y_vec), td::ColorID::Crimson);

    setCentralView(&_mainView);
    
    for (int i = 0; i < broj; i++) {
        y[i] = (i > 100) * 1000 + (i > 300) * 4000 + (i > 600) * 5000;
    }
  
    _mainView.dodajFunkciju(std::move(x), std::move(y), broj);

    _mainView.setBackgroundColor(td::ColorID::Wheat);

}


MainWindow::~MainWindow(){}







