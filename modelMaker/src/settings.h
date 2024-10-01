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

class SettingsView: public gui::View{

    gui::VerticalLayout _vl;

    mu::IAppProperties *props;
    gui::Label _lblLang;
    gui::GridLayout layout;
    gui::ComboBox langCombo;
    const cnt::SafeFullVector<gui::Lang> langs;
    int currentLangIndex = -1;
    gui::Frame *mainWindow;

    gui::CheckBox _chBoxEmbed;
    bool embededCurrent;

    double width = 270;

public:

    SettingsView(gui::Frame *MainWindow):
        mainWindow(MainWindow),
        _lblLang(tr("Laungage: ")),
        layout(1,2),
        props(getAppProperties()),
        _vl(2),
        _chBoxEmbed(tr("embedGraph")),
        embededCurrent(props->getKeyValue("EmbedGraph", "False").strVal().cCompare("False") != 0),
        langs(getSupportedLanguages())
    {
        
        auto currentLang = props->getKeyValue("Laungage", "BA");


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
        _chBoxEmbed.onClick([this](){props->setKeyValue("EmbedGraph", _chBoxEmbed.isChecked());});

        props->setKeyValue("EmbedGraph", 5);

        auto a = props->getKeyValue("EmbedGraph", "False").strVal(); //dodaj checkboy za step time

        width = std::max(width, w);

        gui::GridComposer gc(layout);
        gc.appendRow(_lblLang) << langCombo;


        _vl << layout << _chBoxEmbed;

        setSizeLimits(width, gui::Control::Limit::Fixed); //pokvareno
        setLayout(&_vl);
    }

    ~SettingsView(){
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