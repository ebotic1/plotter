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


class NiceButton : public gui::DrawableButton{

    gui::DrawableString text;
    inline static gui::Font font;
    inline static bool fontInit = false;
    gui::Size size;
    gui::Shape shape;
    td::ColorID color;
    gui::Rect boundingRect;

public:

    NiceButton(const td::String &ButtonText, td::ColorID color = td::ColorID::SysCtrlBack):
        color(color),
        text(ButtonText)
    {
        if(!fontInit){
            fontInit = true;
            font.create("Serif", 10, gui::Font::Style::Bold, gui::Font::Unit::Point);
        }
        constexpr float lineWidth = 5;

        text.measure(&font, size);
        size.width *= 0.35;
        size.width += lineWidth;
        size.height += lineWidth;
        setPreferedContentSize(size);
        setSize(size);

        boundingRect.setOrigin({lineWidth,lineWidth});
        boundingRect.setSize(size);
        shape.createRoundedRect(boundingRect, 20, lineWidth);
    }

    void onDraw(const gui::Rect& rect) override{
        
        shape.drawFillAndWire(color, td::ColorID::SysText);
        text.draw(boundingRect, &font, td::ColorID::SysText, td::TextAlignment::Center, td::VAlignment::Center);

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
        t.setFontSize(20);
        t.setFlat();
        t.setAsReadOnly();
        t.setPreferedContentSize({0,0}); //trebalo bi da ne rasteze textEdit element maksimalno nakon ovoga
        t.setSize({0,0});
        t.sizeToFit();

    }

public:

    StartingView():
        gui::ViewScroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide),
        layoutMain(5), layoutHorizontal(2), layoutGraph(4), layoutText(4),
        buttons{{"Open from file"}, {tr("emptyModel")}, {"\tDAE\t"}, {tr("emptyModel")}, {"Damped sine"}}
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