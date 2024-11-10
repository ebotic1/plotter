#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>
#include <gui/Label.h>
#include <unordered_set>
#include "td/StringConverter.h"

class MainWindow;
class GlobalEvents;



struct SettingsVars
{

	static const char *tr(const char *key){
		static gui::Label helper("");
		return helper.tr(key).c_str();
	}

	static const std::unordered_set<td::String> MonospaceFonts;

	std::vector<const char*> colorNames;
	static constexpr int colorCnt = 8;

	td::ColorID colorAttribute, colorVariable, colorImports, colorKeyword, colorFunctions, colorParam, colorComment, colorConstants, colorText;

	td::ColorID colorsWhite[colorCnt], colorsBlack[colorCnt];

	double textSize;
	td::String font;
	bool embedPlot, restoreTabs, warnBeforeClose;
	
	void saveValues();
	void loadSettingsVars(gui::Application* app);
	void loadDefaultSettings(gui::Application* app);
private:
	void setColors();
	static gui::Application *app;
	friend class GlobalEvents;
};


class GlobalEvents {

public:
	static MainWindow *getMainWindowPtr();
	static bool sendActionItem(gui::ActionItemDescriptor& a);
	static SettingsVars settingsVars;
};