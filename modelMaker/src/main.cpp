#include <gui/WinMain.h>
#include <gui/Application.h>
#include <gui/Window.h>
#include <td/Variant.h>
#include <td/String.h>
#include "td/StringConverter.h" //ne znam zasto ovo mora biti ukljuceno
#include "mainWindow.h"
#include "globalEvents.h"


MainWindow* mainWindow = nullptr;

bool GlobalEvents::sendActionItem(gui::ActionItemDescriptor& a)
{
	if (mainWindow == nullptr)
		return false;
	return mainWindow->onActionItem(a);
}


class application : public gui::Application {
public:
	application(int argc, const char** argv) : gui::Application(argc, argv) {

	}

	gui::Window* createInitialWindow() override{
		return mainWindow = new MainWindow();
		
	}

};

int main(int argc, const char** argv) {
	auto app = new application(argc, argv);

	app->init(app->getProperties()->getKeyValue("Laungage", "BA").toString());
	return app->run();

}