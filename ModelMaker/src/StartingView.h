#pragma once
#include <gui/View.h>
#include <gui/ViewScroller.h>
#include <gui/Label.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/Font.h>
#include <gui/TextEdit.h>
#include <gui/LineEdit.h>
#include <gui/Shape.h>

#include <gui/DrawableButton.h>
#include <gui/DrawableString.h>
#include <gui/_aux/CellInfo.h>

#include "MenuBar.h"
#include "GlobalEvents.h"



class NiceButton : public gui::DrawableButton{

    gui::DrawableString text;
    inline static gui::Font::ID font = gui::Font::ID::SystemLargerBold;
    gui::Size size;
    gui::Shape shape;
    td::ColorID color;
    gui::Rect boundingRect;
    bool openPath = false;
    std::vector<td::String> _pathsToModels;
    static constexpr float lineWidth = 5;

    gui::ActionItemDescriptor action;

public:



    NiceButton(const td::String &ButtonText, int subMenuID, int actionID, td::ColorID color = td::ColorID::SysCtrlBack);
    NiceButton(const td::String &ButtonText, std::vector<td::String> pathsToModels, td::ColorID color = td::ColorID::SysCtrlBack);
    
    void onDraw(const gui::Rect& rect) override;
    bool onClick(gui::DrawableButton* pDrawableBtn) override;
    void measure(gui::CellInfo& cell) override;
 
};


class TextEdit : public gui::TextEdit {

public:
    TextEdit():
        gui::TextEdit() {

    }

    void measure(gui::CellInfo& cell) override {
        gui::TextEdit::measure(cell);
        cell.nResVer = 0;
        cell.minVer = 100;
    }

};


class StartingView: public gui::ViewScroller
{
    TextEdit _labelStartExplain;
    TextEdit _labelTextEditor, _labelGraphicalEditor, _labelExamples;

    NiceButton buttons[12];
    gui::HorizontalLayout _layoutButtons, _layoutButtonsExamples;
    gui::VerticalLayout layoutMain, layoutGraph, layoutText, _layoutExamplesText, _layoutExamplesGraph;
    gui::View helperView;


    void setUpTextEdit(gui::TextEdit &t, const td::String &string);

public:

    StartingView();

    void measure(gui::CellInfo& cell) override {
        gui::ViewScroller::measure(cell);
        //cell.nResVer = 0;
    }

};
