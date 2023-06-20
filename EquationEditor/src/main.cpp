#include "gui/WinMain.h"
#include "gui/Application.h"
#include "gui/Window.h"
#include "mainWindow.h"

class application : public gui::Application {
public:
	application(int argc, const char** argv) : gui::Application(argc, argv) {

	}

	gui::Window* createInitialWindow() override{
		return new MainWindow();
	}

};

int main(int argc, const char** argv) {
	auto app = new application(argc, argv);

	app->init("BA");
	
	return app->run();

}