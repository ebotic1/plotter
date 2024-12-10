#pragma once
#include <gui/ActionItemDescriptor.h>
#include <gui/Window.h>
#include <gui/Label.h>
#include <unordered_set>
#include "td/StringConverter.h"


class MainWindow;
class GlobalEvents;


enum class ColorType : td::BYTE {Constant=0, Function, Attribute, Keyword, Variable, Param, Comment, Import, Text, NONE};

enum class DocumentType : td::BYTE {Unknown = 0, ModelTxtEditor, ModelTFEditor, Graph};

struct SettingsVars
{
    //IDz: Koristiti gui::tr Nema potrebe za kreiranjem dummy objekata
//	static const char *tr(const char *key){
//		static gui::Label helper("");
//		return helper.tr(key).c_str();
//	}

	//static const std::unordered_set<td::String> MonospaceFonts;

    //IDz: Ako se koriste const expr
	//std::vector<const char*> colorNames;
    static constexpr int colorCnt = (int) ColorType::NONE;
    cnt::Array<td::String, colorCnt> colorNames;

//	td::ColorID colorAttribute, colorVariable, colorImports, colorKeyword, colorFunctions, colorParam, colorComment, colorConstants, colorText;

	td::ColorID colorsLight[colorCnt], colorsDark[colorCnt];
    td::ColorID colors[colorCnt]; //used in code to match those in settings

	double textSize;
	td::String font;
	bool embedPlot, restoreTabs, warnBeforeClose;
#ifdef MU_MACOS
    td::BYTE tabStopSizeInChars = 2;
#else
    td::BYTE tabStopSizeInChars = 2;
#endif
	
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

enum class UserEvent: td::UINT4 {ReformatTabs=0, SetModifiedFlag, CloseTab};
