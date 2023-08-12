#include "gui/Window.h"
#include "view.h"

class mainWindow : public gui::Window {
protected:
mainView view;
kanvas k;

public:
	mainWindow() : gui::Window(gui::Size(1000, 1000)) {
		setTitle("ovo ne radi nista??");
	
		setCentralView(&k);
	}
};



