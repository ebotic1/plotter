#include "StartingView.h"
#include "MainWindow.h"

NiceButton::NiceButton(const td::String &ButtonText, int subMenuID, int actionID, td::ColorID color):
        color(color),
        text(ButtonText),
        action(subMenuID, subMenuID, 0, actionID, nullptr)
    {
        text.measure(font, size);
        size.width += 60;
        size.height += 30;


        setSizeLimits(size.width + lineWidth , gui::Control::Limit::Fixed, size.height + lineWidth, gui::Control::Limit::Fixed);

        boundingRect.setOrigin({lineWidth/2,lineWidth/2});
        boundingRect.setSize({size.width, size.height});
        shape.createRoundedRect(boundingRect, 20, lineWidth);
        boundingRect.setSize(size);
    }

 

    NiceButton::NiceButton(const td::String &ButtonText, std::vector<td::String> pathsToModels, td::ColorID color) : NiceButton(ButtonText, 0, 0, color)
    {
        openPath = true;
        _pathsToModels = pathsToModels;
    }

    void NiceButton::onDraw(const gui::Rect &rect)
    {
        
        shape.drawFillAndWire(color, td::ColorID::SysText);
        text.draw(boundingRect, font, td::ColorID::SysText, td::TextAlignment::Center, td::VAlignment::Center);

    }

    bool NiceButton::onClick(gui::DrawableButton *pDrawableBtn)
    {
        if(!openPath){
            GlobalEvents::sendActionItem(action);
            return true;
        }

        for(const auto &path : _pathsToModels)
            GlobalEvents::getMainWindowPtr()->openFile(path);
        return true;
    }

    void NiceButton::measure(gui::CellInfo &cell) {
        gui::DrawableButton::measure(cell);
        cell.nResVer = 0;
        cell.minVer = size.height + lineWidth;
    }

    void StartingView::setUpTextEdit(gui::TextEdit &t, const td::String &string){
        t.setText(string);
        t.setFontSize(16);
        t.setFlat();
        t.setAsReadOnly();
        t.setSizeLimits(0, gui::Control::Limit::None, 50, gui::Control::Limit::Fixed);
    }

    StartingView::StartingView():
        gui::ViewScroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide),
        layoutMain(6), layoutHorizontal(2), layoutGraph(5), layoutText(12),
        buttons{
            {tr("openFromFile"), subMenuNewModel, menuBarActionIDs::OpenFromFile}, 
            {tr("emptyModel"), subMenuNewGraphical , menuBarActionIDs::EmptyModel}, 
            {tr("emptyModel"), subMenuNewText, menuBarActionIDs::EmptyModel},
            {tr("odeModel"), subMenuNewText, menuBarActionIDs::ODE},
            {tr("nrModel"), subMenuNewText, menuBarActionIDs::NR},
            {tr("wlsModel"), subMenuNewText, menuBarActionIDs::WLS},
            {tr("daeModel"), subMenuNewText, menuBarActionIDs::DAE},
            //examples
             {"test", {gui::getResFileName(":WLS")}}
            }
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
        appendButton(buttons[7], layoutGraph);
        layoutGraph.appendSpacer();
        

        layoutText << _labelTextEditor;
        appendButton(buttons[2], layoutText);
        appendButton(buttons[3], layoutText);
        appendButton(buttons[4], layoutText);
        appendButton(buttons[5], layoutText);
        appendButton(buttons[6], layoutText);
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
