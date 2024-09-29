#include "settings.h"
#include <gui/Dialog.h>
#include <gui/Application.h>


class settingsDialog: public gui::Dialog{
    SettingsView view;
public:

    settingsDialog(gui::Frame *frame):
        view(frame),
        gui::Dialog(frame, {
            {gui::Dialog::Button::ID::OK, tr("Ok"), gui::Button::Type::Default}
        }, {0,0})
    {
        setTitle(tr("settings"));
        setCentralView(&view);
    }

    ~settingsDialog(){
        
    }

};