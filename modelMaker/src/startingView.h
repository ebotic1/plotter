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

#include "MenuBar.h"
#include "globalEvents.h"


class NiceButton : public gui::DrawableButton{

    gui::DrawableString text;
    inline static gui::Font font;
    inline static bool fontInit = false;
    gui::Size size;
    gui::Shape shape;
    td::ColorID color;
    gui::Rect boundingRect;

    gui::ActionItemDescriptor action;

public:

    NiceButton(const td::String &ButtonText, int subMenuID, int actionID, td::ColorID color = td::ColorID::SysCtrlBack):
        color(color),
        text(ButtonText),
        action(0, subMenuID, 0, actionID, nullptr)
    {
        
        if(!fontInit){
            fontInit = true;
            font.create("Serif", 10, gui::Font::Style::Bold, gui::Font::Unit::Point);
        }
        constexpr float lineWidth = 5;

        text.measure(&font, size);
        size.width *= 0.35;


        setSizeLimits(size.width + lineWidth, gui::Control::Limit::Fixed, size.height + lineWidth, gui::Control::Limit::Fixed);

        boundingRect.setOrigin({lineWidth/2,lineWidth/2});
        boundingRect.setSize(size);
        shape.createRoundedRect(boundingRect, 20, lineWidth);
    }

    void onDraw(const gui::Rect& rect) override{
        
        shape.drawFillAndWire(color, td::ColorID::SysText);
        text.draw(boundingRect, &font, td::ColorID::SysText, td::TextAlignment::Center, td::VAlignment::Center);

    }


    bool onClick(gui::DrawableButton* pDrawableBtn) {
        GlobalEvents::sendActionItem(action);
        return true;
    }

};


class StartingView: public gui::ViewScroller
{
    gui::TextEdit _labelStartExplain;
    gui::TextEdit _labelTextEditor, _labelGraphicalEditor;

    NiceButton buttons[5];
    gui::HorizontalLayout layoutHorizontal;
    gui::VerticalLayout layoutMain, layoutGraph, layoutText;
    gui::View helperView;


    void setUpTextEdit(gui::TextEdit &t, const td::String &string){
        t.setText(string);
        t.setFontSize(11);
        t.setFlat();
        t.setAsReadOnly();
        t.setSizeLimits(0, gui::Control::Limit::None, 50, gui::Control::Limit::Fixed);
    }

public:

    StartingView():
        gui::ViewScroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide),
        layoutMain(5), layoutHorizontal(2), layoutGraph(4), layoutText(4),
        buttons{
            {tr("openFromFile"), subMenuNewModel, menuBarActionIDs::OpenFromFile}, 
            {tr("emptyModel"), subMenuNewGraphical , menuBarActionIDs::EmptyModel}, 
            {"\tDAE\t", subMenuNewGraphical, 0},
            {tr("emptyModel"), subMenuNewText, menuBarActionIDs::EmptyModel},
            {"Damped sine", subMenuNewText, 0} }
    {   
        setUpTextEdit(_labelStartExplain, tr("startingExplanation"));
        setUpTextEdit(_labelGraphicalEditor, tr("graphicalEditorLabel"));
        setUpTextEdit(_labelTextEditor, tr("textEditorLabel"));

        
        

        layoutGraph << _labelGraphicalEditor;
        layoutGraph << buttons[1] << buttons[2];
        //layoutGraph.appendSpacer();
        

        layoutText << _labelTextEditor;
        layoutText << buttons[3] << buttons[4]; 
        //layoutText.appendSpacer();

        layoutHorizontal << layoutText << layoutGraph;

        layoutMain << _labelStartExplain;
        layoutMain.appendSpace(10);
        layoutMain << buttons[0] << layoutHorizontal;
        //layoutMain.appendSpacer();
        
        helperView.setLayout(&layoutMain);
        setContentView(&helperView);
    }

};