#pragma once
#include "gui/Application.h"
#include "gui/WinMain.h"
#include "mainWindow.h"



class application : public gui::Application {

	gui::Window* createInitialWindow() override {
		return new mainWindow();
	}

public:
	application(int argc, const char** argv): gui::Application(argc, argv){
		
	}

};

int main(int argcnt, const char **argv) {

	application app(argcnt, argv);
	app.init("BA");

	return app.run();

}