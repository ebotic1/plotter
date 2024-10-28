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


    td::ColorID *colors = app->isDarkMode() ? colorsBlack : colorsWhite;

    colorConstants = colors[0];
    colorFunctions = colors[1];
    colorAttribute = colors[2];
    colorKeyword = colors[3];
    colorVariable = colors[4];
    colorParam = colors[5];
    colorComment = colors[6];

    auto &props = *this->app->getProperties(); 
    props.setValue<int>("constantsColorsWhiteEmir", (int)colorsWhite[0]);
    props.setValue<int>("functionColorsWhiteEmir", (int)colorsWhite[1]);
    props.setValue<int>("attribColorWhiteEmir", (int)colorsWhite[2]);
    props.setValue<int>("keywordColorWhiteEmir", (int)colorsWhite[3]);
    props.setValue<int>("variableColorWhiteEmir", (int)colorsWhite[4]);
    props.setValue<int>("paramColorWhiteEmir", (int)colorsWhite[5]);
    props.setValue<int>("commentColorWhiteEmir", (int)colorsWhite[6]);

    props.setValue<int>("constantsColorsBlackEmir", (int)colorsBlack[0]);
    props.setValue<int>("functionColorsBlackEmir", (int)colorsBlack[1]);
    props.setValue<int>("attribColorBlackEmir", (int)colorsBlack[2]);
    props.setValue<int>("keywordColorBlackEmir", (int)colorsBlack[3]);
    props.setValue<int>("variableColorBlackEmir", (int)colorsBlack[4]);
    props.setValue<int>("paramColorBlackEmir", (int)colorsBlack[5]);
    props.setValue<int>("commentColorBlackEmir", (int)colorsBlack[6]);

    props.setValue<int>("importsColorEmir", (int)colorsWhite[0]);
    props.setValue<float>("textSizeEmir", textSize);
    props.setValue<td::String>("editorFontEmir", font);
    props.setValue<int>("embedPlotEmir", (int)(embedPlot ? 1 : 0));
    props.setValue<int>("restoreTabsEmir", (int)(restoreTabs ? 1 : 0));
    props.setValue<int>("warnBeforeClosingEmir", (int)(warnBeforeClose ? 1 : 0));


}
