#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>

class MainWindow;
class GlobalEvents;

struct SettingsVars
{
	td::ColorID colorAttribute, colorVariable, colorImports, colorKeyword, colorParam, colorComment;
	double textSize;
	void saveValues();
private:
	static gui::Application *app;
	friend class GlobalEvents;
};


class GlobalEvents {

public:
	static MainWindow *getMainWindowPtr();
	static bool sendActionItem(gui::ActionItemDescriptor& a);
	static void loadSettingsVars(gui::Application *app);
	static SettingsVars settingsVars;
};