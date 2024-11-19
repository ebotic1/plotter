#pragma once
#include <gui/ToolBar.h>
#include "../../Graph/inc/Graph.h"


class ToolBar: public gui::ToolBar{
    gui::Image _imgs[9];
    static constexpr int cnt = sizeof(_imgs) / sizeof(gui::Image) + 0.5;
public:

    ToolBar():
    gui::ToolBar("main", cnt)
    {
        if(cnt > Graph::buttonsImages.size()){
            assert("Invalid toolbar");
            throw("Invalid toolbar");
        }
        const char *labels[cnt] = {"Minimize margins", "Show grid", "Show legend", "Show annotations", "Save", "Delete Annotations", "Fit to window", "Help", "Settings"};
        const char *desc[cnt] = {"Set margins to zero", "Show grid lines", "Toggle plot legend", "Show lines added with v/h", \
        "Save as data or image", "Delete all lines added with v/h", "Fit curve to window", "Show keyboard shortcuts", "Plot settings"};

        for(int i = 0; i<cnt; ++i){
            if(i == 7 || i==4) //preskakanje nekih ikona
                continue;
            _imgs[i].load(Graph::buttonsImages[i]);
            addItem(labels[i], _imgs + i, desc[i], 100, 0, 0, i);
        }
    }

};