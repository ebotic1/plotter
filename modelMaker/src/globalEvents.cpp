#include "globalEvents.h"
#include <gui/Application.h>

SettingsVars GlobalEvents::settingsVars;
gui::Application *SettingsVars::app = nullptr;

void GlobalEvents::loadSettingsVars(gui::Application *app)
{
    SettingsVars::app = app;
    auto &props = *app->getProperties();

    settingsVars.colorAttribute = props.getKeyValue("attribColor", td::ColorID::LightSkyBlue).colorID();
    settingsVars.colorImports = props.getKeyValue("importsColor", td::ColorID::Teal).colorID();
    settingsVars.colorKeyword =  props.getKeyValue("keywordColor", td::ColorID::DodgerBlue).colorID();
    settingsVars.colorVariable =  props.getKeyValue("variableColor", td::ColorID::MediumSeaGreen).colorID();
    settingsVars.colorParam =  props.getKeyValue("paramColor", td::ColorID::OrangeRed).colorID();
    settingsVars.colorComment = props.getKeyValue("commentColor", td::ColorID::LimeGreen).colorID();
    settingsVars.textSize = props.getValue<double>("textSize", 14.0);

}

void SettingsVars::saveValues()
{
    if(app == nullptr || true)
        return;
    auto &props = *app->getProperties();

    props.setValue<td::ColorID>("attribColor", colorAttribute);
    props.setValue<td::ColorID>("importsColor", colorImports);
    props.setValue<td::ColorID>("keywordColor", colorKeyword);
    props.setValue<td::ColorID>("variableColor", colorVariable);
    props.setValue<td::ColorID>("paramColor", colorKeyword);
    props.setValue<td::ColorID>("commentColor", colorVariable);
    props.setValue<double>("textSize", textSize);


}
