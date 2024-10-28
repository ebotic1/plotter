#pragma once
#include <gui/View.h>
#include <gui/Label.h>
#include <gui/ComboBox.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include <mu/IAppProperties.h>
#include <gui/Application.h>
#include <gui/VerticalLayout.h>
#include <gui/CheckBox.h>
#include <gui/TabView.h>

class SettingsView: public gui::View{

    gui::View _vMain, _vColors;
    gui::VerticalLayout _vl, _tabViewLayout;
    gui::TabView _tabView;
    gui::Image _settingsImg;

    mu::IAppProperties *props;
    gui::Label _lblLang;
    gui::GridLayout layout, _colorsGrid;
    gui::ComboBox langCombo;
    const cnt::SafeFullVector<gui::Lang> langs;
    int currentLangIndex = -1;
    gui::Frame *mainWindow;

    gui::CheckBox _chBoxEmbed, _chBoxRestoreTabs, _chBoxConfirmClose;

    gui::Label _lblFont;
    gui::ComboBox fontCombo;

    bool embededCurrent;

    double width = 270;

public:

    SettingsView(gui::Frame *MainWindow):
        mainWindow(MainWindow),
        _lblLang(tr("Laungage: ")),
        layout(2,2),
        props(getAppProperties()),
        _vl(4),
        _tabViewLayout(1),
        _colorsGrid(14, 2),
        _chBoxEmbed(tr("embedGraph")),
        _chBoxRestoreTabs(tr("restoreTabs")),
        _chBoxConfirmClose(tr("confirmCloseSetting")),
        embededCurrent(GlobalEvents::settingsVars.embedPlot),
        langs(getSupportedLanguages()),
        _lblFont(tr("fontLabel")),
        _tabView(gui::TabHeader::Type::FitToText, 20, 20),
        _settingsImg(":settings")
    {
        
        auto currentLang = props->getKeyValue("Laungage", "BA");
        auto &settings = GlobalEvents::settingsVars;

        for(int i = 0; i<langs.size(); ++i){
            langCombo.addItem(langs[i].getDescription());
            if(currentLang == langs[i].getExtension()){
                langCombo.selectIndex(langCombo.getNoOfItems() - 1);
                currentLangIndex = i;
            }
        }

        gui::CoordType w = langCombo.getWidthToFitLongestItem();
        langCombo.setSizeLimits(w, gui::Control::Limit::Fixed);

        _chBoxEmbed.setChecked(embededCurrent, false);
        _chBoxEmbed.onClick([this, &settings](){settings.embedPlot = _chBoxEmbed.isChecked();});

        _chBoxRestoreTabs.setChecked(settings.restoreTabs);
        _chBoxRestoreTabs.onClick([this, &settings](){settings.restoreTabs = _chBoxRestoreTabs.isChecked();});

        _chBoxConfirmClose.setChecked(settings.warnBeforeClose);
        _chBoxConfirmClose.onClick([this, &settings](){settings.warnBeforeClose = _chBoxConfirmClose.isChecked();});
        
        auto fonts = gui::Font::getSystemFamilyNames();
        fontCombo.addItem("Default");
        fontCombo.addItems(fonts.begin(), fonts.size());
        fontCombo.setSizeLimits(fontCombo.getWidthToFitLongestItem(), gui::Control::Limit::Fixed);
        w = std::max(w, fontCombo.getWidthToFitLongestItem());

        fontCombo.selectIndex(0);
        for(int i = 0; i<fonts.size(); ++i)
            if(fonts[i] == settings.font){
                fontCombo.selectIndex(i+1);
                break;
            }


        width = std::max(width, w);

        gui::GridComposer gc(layout);
        gc.appendRow(_lblLang) << langCombo;
        gc.appendRow(_lblFont) << fontCombo;


        _vl << layout << _chBoxEmbed << _chBoxRestoreTabs << _chBoxConfirmClose;

        setSizeLimits(width, gui::Control::Limit::Fixed);

        _vMain.setLayout(&_vl);
        _vColors.setLayout(&_colorsGrid);

        _tabView.addView(&_vMain, tr("appSettings"), &_settingsImg);
        _tabView.addView(&_vColors, tr("colorsSettings"), &_settingsImg);
        

        _tabViewLayout << _tabView;
        setLayout(&_tabViewLayout);
    }

    ~SettingsView(){
        GlobalEvents::settingsVars.font = fontCombo.getSelectedText();


        GlobalEvents::settingsVars.saveValues();
        bool restartRequired = false;
        int selectedLang = langCombo.getSelectedIndex();
        if(selectedLang != currentLangIndex){
            props->setKeyValue("Laungage", langs[selectedLang].getExtension());
            restartRequired = true;
        }

        if(_chBoxEmbed.getValue() != embededCurrent){

            restartRequired = true;
        }

        if(restartRequired)
            mainWindow->showYesNoQuestionAsync(tr("RestartRequired"), tr("RestartRequiredInfo"), tr("Restart"), tr("DoNoRestart"), [this] (gui::Alert::Answer answer) {
            if (answer == gui::Alert::Answer::Yes)
            {
                getApplication()->restart();
            }
            });

    }

};