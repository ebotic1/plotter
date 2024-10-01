#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>

class MainWindow;

class GlobalEvents {

public:
	static MainWindow *getMainWindowPtr();
	static bool sendActionItem(gui::ActionItemDescriptor& a);
};