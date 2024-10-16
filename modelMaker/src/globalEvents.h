#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>

class MainWindow;
class GlobalEvents;

struct SettingsVars
{
	td::ColorID colorAttribute, colorVariable, colorImports, colorKeyword, colorParam, colorComment;
	double textSize;
	td::String font;
	bool embedPlot;
	void saveValues();
	void loadSettingsVars(gui::Application* app);
	void loadDefaultSettings(gui::Application* app);
private:
	static gui::Application *app;
	friend class GlobalEvents;
};


class GlobalEvents {

public:
	static MainWindow *getMainWindowPtr();
	static bool sendActionItem(gui::ActionItemDescriptor& a);
	static SettingsVars settingsVars;
};