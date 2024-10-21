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


    colorAttribute = td::ColorID(props.getValue<int>("attribColorEmir", (int)td::ColorID::LightSkyBlue));
    colorImports = td::ColorID(props.getValue<int>("importsColorEmir", (int)td::ColorID::Teal));
    colorKeyword = td::ColorID(props.getValue<int>("keywordColorEmir", (int)td::ColorID::DodgerBlue));
    colorVariable = td::ColorID(props.getValue<int>("variableColorEmir", (int)td::ColorID::MediumSeaGreen));
    colorParam = td::ColorID(props.getValue<int>("paramColorEmir", (int)td::ColorID::OrangeRed));
    colorComment = td::ColorID(props.getValue<int>("commentColorEmir", (int)td::ColorID::LimeGreen));
    textSize = props.getValue<float>("textSizeEmir", 14.0);
    font = props.getValue<td::String>("editorFontEmir", "Default");
    embedPlot = (props.getValue<int>("embedPlotEmir", 1) == 1);

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

    props.setValue<int>("attribColorEmir", (int)colorAttribute);
    props.setValue<int>("importsColorEmir", (int)colorImports);
    props.setValue<int>("keywordColorEmir", (int)colorKeyword);
    props.setValue<int>("variableColorEmir", (int)colorVariable);
    props.setValue<int>("paramColorEmir", (int)colorParam);
    props.setValue<int>("commentColorEmir", (int)colorComment);
    props.setValue<float>("textSizeEmir", textSize);
    props.setValue<td::String>("editorFontEmir", font);
    props.setValue<int>("embedPlotEmir", (int)(embedPlot ? 1 : 0));


}
