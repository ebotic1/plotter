#include "GlobalEvents.h"
#include <gui/Application.h>

SettingsVars GlobalEvents::settingsVars;
gui::Application *SettingsVars::app = nullptr;


void SettingsVars::loadSettingsVars(gui::Application* app)
{
    if (this->app == nullptr)
    {
        if (app != nullptr)
            this->app = app;
        else
            return;
    }
    auto &props = *this->app->getProperties();




    colorsWhite[0] = td::ColorID(props.getValue<int>("constantsColorsWhiteEmir", (int)td::ColorID::Red));
    colorsWhite[1] = td::ColorID(props.getValue<int>("functionColorsWhiteEmir", (int)td::ColorID::DeepSkyBlue));
    colorsWhite[2] = td::ColorID(props.getValue<int>("attribColorWhiteEmir", (int)td::ColorID::LightSkyBlue));
    colorsWhite[3] = td::ColorID(props.getValue<int>("keywordColorWhiteEmir", (int)td::ColorID::DodgerBlue));
    colorsWhite[4] = td::ColorID(props.getValue<int>("variableColorWhiteEmir", (int)td::ColorID::MediumSeaGreen));
    colorsWhite[5] = td::ColorID(props.getValue<int>("paramColorWhiteEmir", (int)td::ColorID::OrangeRed));
    colorsWhite[6] = td::ColorID(props.getValue<int>("commentColorWhiteEmir", (int)td::ColorID::LimeGreen));


    colorsBlack[0] = td::ColorID(props.getValue<int>("constantsColorsBlackEmir", (int)td::ColorID::Red));
    colorsBlack[1] = td::ColorID(props.getValue<int>("functionColorsBlackEmir", (int)td::ColorID::DeepSkyBlue));
    colorsBlack[2] = td::ColorID(props.getValue<int>("attribColorBlackEmir", (int)td::ColorID::LightSkyBlue));
    colorsBlack[3] = td::ColorID(props.getValue<int>("keywordColorBlackEmir", (int)td::ColorID::DodgerBlue));
    colorsBlack[4] = td::ColorID(props.getValue<int>("variableColorBlackEmir", (int)td::ColorID::MediumSeaGreen));
    colorsBlack[5] = td::ColorID(props.getValue<int>("paramColorBlackEmir", (int)td::ColorID::OrangeRed));
    colorsBlack[6] = td::ColorID(props.getValue<int>("commentColorBlackEmir", (int)td::ColorID::LimeGreen));

    td::ColorID *colors = app->isDarkMode() ? colorsBlack : colorsWhite;

    colorConstants = colors[0];
    colorFunctions = colors[1];
    colorAttribute = colors[2];
    colorKeyword = colors[3];
    colorVariable = colors[4];
    colorParam = colors[5];
    colorComment = colors[6];
    


    colorImports = td::ColorID(props.getValue<int>("importsColorEmir", (int)td::ColorID::Teal));
    textSize = props.getValue<float>("textSizeEmir", 14.0);
    font = props.getValue<td::String>("editorFontEmir", "Default");
    embedPlot = (props.getValue<int>("embedPlotEmir", 1) == 1);
    restoreTabs = (props.getValue<int>("restoreTabsEmir", 1) == 1);
    warnBeforeClose = (props.getValue<int>("warnBeforeClosingEmir", 1) == 1);

}


void SettingsVars::loadDefaultSettings(gui::Application* app)
{
    if (this->app == nullptr)
    {
        if (app != nullptr)
            this->app = app;
        else
            return;
    }

    auto& props = *this->app->getProperties();


    for (const td::String& key : { "attribColorEmir", "importsColorEmir", "keywordColorEmir", "variableColorEmir", "paramColorEmir", "commentColorEmir", "textSizeEmir", "editorFontEmir", "embedPlotEmir" }) 
        props.deleteKey(key);
    
    loadSettingsVars(nullptr);

}

void SettingsVars::saveValues()
{
    if(app == nullptr)
        return;
    auto &props = *this->app->getProperties(); 
    props.setValue<int>("constantsColorsEmir", (int)colorConstants);
    props.setValue<int>("functionColorsEmir", (int)colorFunctions);
    props.setValue<int>("attribColorEmir", (int)colorAttribute);
    props.setValue<int>("importsColorEmir", (int)colorImports);
    props.setValue<int>("keywordColorEmir", (int)colorKeyword);
    props.setValue<int>("variableColorEmir", (int)colorVariable);
    props.setValue<int>("paramColorEmir", (int)colorParam);
    props.setValue<int>("commentColorEmir", (int)colorComment);
    props.setValue<float>("textSizeEmir", textSize);
    props.setValue<td::String>("editorFontEmir", font);
    props.setValue<int>("embedPlotEmir", (int)(embedPlot ? 1 : 0));
    props.setValue<int>("restoreTabsEmir", (int)(restoreTabs ? 1 : 0));
    props.setValue<int>("warnBeforeClosingEmir", (int)(warnBeforeClose ? 1 : 0));


}
