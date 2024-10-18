#pragma once
#include <gui/View.h>
#include <gui/ViewScroller.h>
#include <gui/Label.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/Font.h>
#include <gui/TextEdit.h>
#include <gui/LineEdit.h>

#include <gui/DrawableButton.h>
#include <gui/DrawableString.h>
#include <gui/_aux/CellInfo.h>

#include "MenuBar.h"
#include "globalEvents.h"



class NiceButton : public gui::DrawableButton{

    gui::DrawableString text;
    inline static gui::Font::ID font = gui::Font::ID::SystemLargerBold;
    gui::Size size;
    gui::Shape shape;
    td::ColorID color;
    gui::Rect boundingRect;
    static constexpr float lineWidth = 5;

    gui::ActionItemDescriptor action;

public:

    NiceButton(const td::String &ButtonText, int subMenuID, int actionID, td::ColorID color = td::ColorID::SysCtrlBack):
        color(color),
        text(ButtonText),
        action(subMenuID, subMenuID, 0, actionID, nullptr)
    {
        
        text.measure(font, size); // izbrisati
        text.measure(font, size);
        size.width += 60;
        size.height += 30;


        setSizeLimits(size.width + lineWidth , gui::Control::Limit::Fixed, size.height + lineWidth, gui::Control::Limit::Fixed);

        boundingRect.setOrigin({lineWidth/2,lineWidth/2});
        boundingRect.setSize({size.width, size.height});
        shape.createRoundedRect(boundingRect, 20, lineWidth);
        boundingRect.setSize(size);
    }

    void onDraw(const gui::Rect& rect) override{
        
        shape.drawFillAndWire(color, td::ColorID::SysText);
        text.draw(boundingRect, font, td::ColorID::SysText, td::TextAlignment::Center, td::VAlignment::Center);

    }


    bool onClick(gui::DrawableButton* pDrawableBtn) {
        GlobalEvents::sendActionItem(action);
        return true;
    }


    void measure(gui::CellInfo& cell) override {
        gui::DrawableButton::measure(cell);
        cell.nResVer = 0;
        cell.minVer = size.height + lineWidth;
    }

 
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
    TextEdit _labelTextEditor, _labelGraphicalEditor;

    NiceButton buttons[4];
    gui::HorizontalLayout layoutHorizontal;
    gui::VerticalLayout layoutMain, layoutGraph, layoutText;
    gui::View helperView;


    void setUpTextEdit(gui::TextEdit &t, const td::String &string){
        t.setText(string);
        t.setFontSize(16);
        t.setFlat();
        t.setAsReadOnly();
        t.setSizeLimits(0, gui::Control::Limit::None, 50, gui::Control::Limit::Fixed);
    }

public:

    StartingView():
        gui::ViewScroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide),
        layoutMain(6), layoutHorizontal(2), layoutGraph(4), layoutText(6),
        buttons{
            {tr("openFromFile"), subMenuNewModel, menuBarActionIDs::OpenFromFile}, 
            {tr("emptyModel"), subMenuNewGraphical , menuBarActionIDs::EmptyModel}, 
            {tr("emptyModel"), subMenuNewText, menuBarActionIDs::EmptyModel},
            {tr("odeModel"), subMenuNewText, menuBarActionIDs::ODE} }
    {   
        setUpTextEdit(_labelStartExplain, tr("startingExplanation"));
        setUpTextEdit(_labelGraphicalEditor, tr("graphicalEditorLabel"));
        setUpTextEdit(_labelTextEditor, tr("textEditorLabel"));

        
        auto appendButton = [](NiceButton &b, gui::VerticalLayout &v) {
            v << b;
            v.appendSpace(10);
            };

        layoutGraph << _labelGraphicalEditor;
        appendButton(buttons[1], layoutGraph);
        layoutGraph.appendSpacer();
        

        layoutText << _labelTextEditor;
        appendButton(buttons[2], layoutText);
        appendButton(buttons[3], layoutText);
        layoutText.appendSpacer();

       

        layoutHorizontal << layoutText << layoutGraph;

        layoutMain << _labelStartExplain;
        layoutMain.appendSpace(15);
        layoutMain << buttons[0];
        layoutMain.appendSpace(29);
        layoutMain << layoutHorizontal;
        layoutMain.appendSpacer();
        
        helperView.setLayout(&layoutMain);
        setContentView(&helperView);
    }

    void measure(gui::CellInfo& cell) override {
        gui::ViewScroller::measure(cell);
        //cell.nResVer = 0;
    }

};