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

    //IDz: Ne praviti novi tr (koristiti gui::tr
    colorNames = { gui::tr("constants"), gui::tr("functions"), gui::tr("attributes"),  gui::tr("keywords"), gui::tr("variables"), gui::tr("paramaters"), gui::tr("comments"), gui::tr("importColor"), gui::tr("textColor")};
    
    assert((colorNames.size() ==  colorCnt) && "SettingsVars u globalEvents.h mora imati ispravno podešenu vrijednost colorCnt");
    
    //IDz: Ne ovako!!
//    if(colorNames.size() != colorCnt)
//        assert("SettingsVars u globalEvents.h mora imati ispravno podešenu vrijednost colorCnt");
    

    colorsLight[0] = td::ColorID(props.getValue<int>("constantsColorsLight", (int)td::ColorID::Red));
    colorsLight[1] = td::ColorID(props.getValue<int>("functionColorsLight", (int)td::ColorID::DeepSkyBlue));
    colorsLight[2] = td::ColorID(props.getValue<int>("attribColorLight", (int)td::ColorID::LightSkyBlue));
    colorsLight[3] = td::ColorID(props.getValue<int>("keywordColorLight", (int)td::ColorID::DodgerBlue));
    colorsLight[4] = td::ColorID(props.getValue<int>("variableColorLight", (int)td::ColorID::MediumSeaGreen));
    colorsLight[5] = td::ColorID(props.getValue<int>("paramColorLight", (int)td::ColorID::OrangeRed));
    colorsLight[6] = td::ColorID(props.getValue<int>("commentColorLight", (int)td::ColorID::LimeGreen));
    colorsLight[7] = td::ColorID(props.getValue<int>("importColorLight", (int)td::ColorID::DarkGreen));
    colorsLight[8] = td::ColorID(props.getValue<int>("textColorLight", (int)td::ColorID::SysText));


    colorsDark[0] = td::ColorID(props.getValue<int>("constantsColorsDark", (int)td::ColorID::Violet));
    colorsDark[1] = td::ColorID(props.getValue<int>("functionColorsDark", (int)td::ColorID::Lime));
    colorsDark[2] = td::ColorID(props.getValue<int>("attribColorDark", (int)td::ColorID::LightSkyBlue));
    colorsDark[3] = td::ColorID(props.getValue<int>("keywordColorDark", (int)td::ColorID::DodgerBlue));
    colorsDark[4] = td::ColorID(props.getValue<int>("variableColorDark", (int)td::ColorID::Gold));
    colorsDark[5] = td::ColorID(props.getValue<int>("paramColorDark", (int)td::ColorID::GreenYellow));
    colorsDark[6] = td::ColorID(props.getValue<int>("commentColorDark", (int)td::ColorID::Thistle));
    colorsDark[7] = td::ColorID(props.getValue<int>("importColorDark", (int)td::ColorID::Turquoise));
    colorsDark[8] = td::ColorID(props.getValue<int>("textColorDark", (int)td::ColorID::SysText));

    setColors();

    //colorImports = td::ColorID(props.getValue<int>("importsColor", (int)td::ColorID::Teal));
    float defaultFontSize = gui::Font::getSystemHeight();
    textSize = props.getValue<float>("textSize", defaultFontSize);
    font = props.getValue<td::String>("editorFont", "Default");
    embedPlot = (props.getValue<int>("embedPlot", 1) == 1);
    restoreTabs = (props.getValue<int>("restoreTabs", 1) == 1);
    warnBeforeClose = (props.getValue<int>("warnBeforeClosing", 1) == 1);

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


    for (const td::String& key : { "attribColor", "importsColor", "keywordColor", "variableColor", "paramColor", "commentColor", "textSize", "editorFont", "embedPlot" })
        props.deleteKey(key);
    
    loadSettingsVars(nullptr);

}

void SettingsVars::setColors()
{
    td::ColorID* currColors = app->isDarkMode() ? colorsDark : colorsLight;

    for (int i=0; i<colorCnt; i++)
        colors[i] = currColors[i];
}

void SettingsVars::saveValues()
{
    if(app == nullptr)
        return;

    setColors();

    auto &props = *this->app->getProperties(); 
    props.setValue<int>("constantsColorsLight", (int)colorsLight[0]);
    props.setValue<int>("functionColorsLight", (int)colorsLight[1]);
    props.setValue<int>("attribColorLight", (int)colorsLight[2]);
    props.setValue<int>("keywordColorLight", (int)colorsLight[3]);
    props.setValue<int>("variableColorLight", (int)colorsLight[4]);
    props.setValue<int>("paramColorLight", (int)colorsLight[5]);
    props.setValue<int>("commentColorLight", (int)colorsLight[6]);
    props.setValue<int>("importColorLight", (int)colorsDark[7]);
    props.setValue<int>("textColorLight", (int)colorsLight[8]);

    props.setValue<int>("constantsColorsDark", (int)colorsDark[0]);
    props.setValue<int>("functionColorsDark", (int)colorsDark[1]);
    props.setValue<int>("attribColorDark", (int)colorsDark[2]);
    props.setValue<int>("keywordColorDark", (int)colorsDark[3]);
    props.setValue<int>("variableColorDark", (int)colorsDark[4]);
    props.setValue<int>("paramColorDark", (int)colorsDark[5]);
    props.setValue<int>("commentColorDark", (int)colorsDark[6]);
    props.setValue<int>("importColorDark", (int)colorsDark[7]);
    props.setValue<int>("textColorDark", (int)colorsDark[8]);

    props.setValue<int>("importsColor", (int)colorsLight[0]);
    props.setValue<float>("textSize", textSize);
    props.setValue<td::String>("editorFont", font);
    props.setValue<int>("embedPlot", (int)(embedPlot ? 1 : 0));
    props.setValue<int>("restoreTabs", (int)(restoreTabs ? 1 : 0));
    props.setValue<int>("warnBeforeClosing", (int)(warnBeforeClose ? 1 : 0));
}

//
//#ifdef MU_MACOS
//const std::unordered_set<td::String> SettingsVars::MonospaceFonts = {
//    "Menlo",
//    "Courier",
//    "Courier New",
//    "Lucida Console",
//    "Monaco",
//    "Andale Mono",
//    "Consolas",
//    "DejaVu Sans Mono",
//    "Apple SD Gothic Neo",
//    "Fira Code"
//};
//#else
//const std::unordered_set<td::String> SettingsVars::MonospaceFonts = {
//    "Courier",
//    "Courier New",
//    "Lucida Console",
//    "Consolas",
//    "Monaco",
//    "Menlo",
//    "DejaVu Sans Mono",
//    "Liberation Mono",
//    "Inconsolata",
//    "Source Code Pro",
//    "Fira Code",
//    "IBM Plex Mono",
//    "Droid Sans Mono",
//    "Anonymous Pro",
//    "Ubuntu Mono",
//    "Bitstream Vera Sans Mono",
//    "Hack",
//    "JetBrains Mono",
//    "Cascadia Code",
//    "Noto Mono",
//    "SF Mono",
//    "Terminus",
//    "ProFont",
//    "Input Mono",
//    "Andale Mono",
//    "M+ 1m",
//    "Meslo LG",
//    "Envy Code R",
//    "Go Mono",
//    "Share Tech Mono",
//    "Overpass Mono",
//    "Dina",
//    "PragmataPro",
//    "Camingo Code",
//    "Roboto Mono",
//    "Iosevka",
//    "Victor Mono",
//    "Recursive Mono",
//    "Monoid",
//    "Space Mono"
//};
//#endif