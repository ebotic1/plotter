#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>

class MainWindow;
class GlobalEvents;



struct SettingsVars
{

	static constexpr const char* colorNames[] = { "Constants", "Functions", "Attributes",  "Keywords", "Variables", "Paramaters", "Comments", "Text Color"};

	td::ColorID colorAttribute, colorVariable, colorImports, colorKeyword, colorFunctions, colorParam, colorComment, colorConstants, colorText;

	td::ColorID colorsWhite[8], colorsBlack[8];

	double textSize;
	td::String font;
	bool embedPlot, restoreTabs, warnBeforeClose;
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