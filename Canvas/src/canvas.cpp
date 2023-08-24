#include "./../inc/canvas.h"
#include "gui/Shape.h"
#include "gui/DrawableString.h"

#define SELECT_COLOR td::ColorID::AliceBlue
#define FONT gui::Font::ID::ReportBody1

const std::initializer_list<gui::InputDevice::Event> graph::inputs = { gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag,
gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave };

const std::initializer_list<gui::InputDevice::Event> graph::noInputs = {};


class graph::legend {
    std::vector<gui::DrawableString> imena;
    std::vector<td::ColorID> colors;
    gui::CoordType length = 0;

    static constexpr double offset = 13;
    static constexpr double rectSize = 21;

    int cnt = 0;
public:
    td::ColorID textColor;
    legend(td::ColorID textColor) : textColor(textColor), imena(10) {};

    void addFunction(const Function &f) {
        imena[cnt] = f.name.c_str(); // zaobilazi se koristenje move i copy konstruktora drawableString na ovaj nacin dok se ne poprave konstruktori. UPDATE: ne radi ni ovo uvijek
        gui::Size sz;
        imena[cnt].measure(FONT, sz);
        ++cnt;
        if (length < sz.width)
            length = sz.width;
        colors.emplace_back(f.getColor());
    };

    void draw(const gui::Point& topRight) {
        gui::CoordType height = topRight.y;
        for (size_t i = 0; i < colors.size(); ++i) {
            imena[i].draw({ topRight.x - length, height}, FONT, textColor);
            gui::Shape::drawRect(gui::Rect({ topRight.x - length - offset - rectSize, height }, gui::Size(rectSize, rectSize)), colors[i]);
            height += rectSize + 10;
        }
    }
};

graph::graph(bool startWithMargins, bool takeUserInput, td::ColorID backgroundColor) :gui::Canvas(takeUserInput ? inputs : noInputs), backgroundColor(backgroundColor), drawMargins(startWithMargins)
{

    enableResizeEvent(true);

    if (backgroundColor == axisColor)
        axisColor = td::ColorID::Black;
    if (backgroundColor == axisColor)
        axisColor = td::ColorID::White;
    setBackgroundColor(backgroundColor);

    gui::DrawableString test("1234567890.1234567890e^(1234567890.1234567890)");
    gui::Size sz;
    test.measure(FONT, sz);
    numberHeight = sz.height;

    legenda = new legend(axisColor);

    if (startWithMargins)
        setUpDrawingWindow();
}

void graph::setUpDrawingWindow(){
    gui::Point past = drawingWindow.point;
    getGeometry(drawingWindow);
    //drawingWindow.size.height += drawingWindow.point.y;
    drawingWindow.point.y = 0;

    if (drawMargins) {
        gui::Point center;
        center.x = drawingWindow.point.x + drawingWindow.size.width / 2;
        center.y = drawingWindow.point.y + drawingWindow.size.height / 2;

        drawingWindow.size.width /= marginsFactor;
        drawingWindow.size.height /= marginsFactor;
        drawingWindow.point.x = center.x - drawingWindow.size.width / 2;
        drawingWindow.point.y = center.y - drawingWindow.size.height / 2;
    }

    for (int i = 0; i < funkcije.size(); ++i) {
        funkcije[i].increaseShiftX(drawingWindow.point.x - past.x);
        funkcije[i].increaseShiftY(past.y - drawingWindow.point.y); // - drawingWindow.size.height);
    }

}



void graph::showMargins(double reductionFactor){
    drawMargins = (reductionFactor >= 1) ? true : false;
    if (drawMargins)
        marginsFactor = reductionFactor;
    setUpDrawingWindow();
    reDraw();
}

void graph::reset(){
    pastColors.clear();
    lastColor = 0;
    action = Actions::none;
    funkcije.clear();
}

void graph::setAxisColor(td::ColorID boja){
    axisColor = boja;
    reDraw();
}

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, double lineWidth, td::LinePattern pattern, td::String name){
    funkcije.emplace_back(x, y, length, color, name, lineWidth, pattern);
    finishAddingFunction(funkcije.back());
}

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth, td::LinePattern pattern, td::String name){
    td::ColorID boja;
    if (lastColor == 0 && pastColors.size() == 0) {
        if (backgroundColor == td::ColorID::Black)
            boja = td::ColorID::White;
        else
            boja = td::ColorID::Black;
    }
    else {
        bool repeat;
        int current = lastColor;
        do {
            repeat = false;
            lastColor += 23;
            lastColor = lastColor % 137; //otprilike sve boje su obuhvacene i svaka boja ce se izabrati prije nego sto se pocnu ponavljati
            for each (td::ColorID boja in pastColors)
                if (lastColor == int(boja) || lastColor == int(backgroundColor)){
                    repeat = true;
                    break;
                }
            if (lastColor == current)
                break;
        } while (repeat);
        boja = td::ColorID(lastColor);
    }

    funkcije.emplace_back(x, y, length, boja, name, lineWidth, pattern);
    finishAddingFunction(funkcije.back());
}

void graph::addFunction(Function&& fun){
    funkcije.emplace_back(std::move(fun));
    finishAddingFunction(funkcije.back());
}

void graph::finishAddingFunction(Function& newFun) {
    updateLimits(newFun);
    pastColors.push_back(newFun.getColor());
    if (funkcije.size() == 1) {
        newFun.increaseScaleAndShiftY(-1, 0);
    }
    else {
        gui::CoordType scaleX, scaleY, shiftX, shiftY;
        funkcije[0].getScale(scaleX, scaleY);
        funkcije[0].getShift(shiftX, shiftY);
        newFun.increaseScaleAndShiftX(scaleX, shiftX);
        newFun.increaseScaleAndShiftY(scaleY, shiftY);
    }

    legenda->addFunction(newFun);
}

void graph::updateLimits(const Function& newFun){
    size_t length = newFun.getLength();
    if (length == 0)
        return;

    const gui::Point *tacke = newFun.getPoints();

    if (Limits == nullptr) {
        Limits = new gui::CoordType[4];
        Limits[int(limits::xMin)] = tacke[0].x;
        Limits[int(limits::xMax)] = tacke[0].x;
        Limits[int(limits::yMin)] = tacke[0].y;
        Limits[int(limits::yMax)] = tacke[0].y;
    }

    for (size_t i = 0; i < length; ++i) {
        if (Limits[int(limits::xMax)] < tacke[i].x)
            Limits[int(limits::xMax)] = tacke[i].x;

        if (Limits[int(limits::yMax)] < tacke[i].y)
            Limits[int(limits::yMax)] = tacke[i].y;

        if (Limits[int(limits::xMin)] > tacke[i].x)
            Limits[int(limits::xMin)] = tacke[i].x;

        if (Limits[int(limits::yMin)] > tacke[i].y)
            Limits[int(limits::yMin)] = tacke[i].y;
    }
}

void graph::onResize(const gui::Size& newSize) {
    setUpDrawingWindow();
    if (!initalDraw)
        fitToWindow();
}

void graph::fitToWindow(){
    if (Limits == nullptr)
        return;
    if (drawingWindow.size.width == 0 || drawingWindow.size.height == 0)
        return;

    initalDraw = true;
    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    funkcije[0].getScale(scaleX, scaleY);
    funkcije[0].getShift(shiftX, shiftY);

    gui::Geometry g;
    g.point.x = Limits[int(limits::xMin)] * scaleX + shiftX;
    g.point.y = Limits[int(limits::yMax)] * scaleY - shiftY;
    g.size.width = (Limits[int(limits::xMax)] - Limits[int(limits::xMin)]) * scaleX;
    g.size.height = (Limits[int(limits::yMin)] - Limits[int(limits::yMax)]) * scaleY;


    ZoomToWindow(g);
}

void graph::ZoomToWindow(const gui::Geometry& window){
    if (funkcije.size() == 0)
        return;
    if (window.size.width < 0.1 || window.size.height < 0.1) {
        return;
    }

    gui::CoordType shiftY = window.point.y + window.size.height - drawingWindow.point.y - drawingWindow.size.height;
    gui::CoordType shiftX = drawingWindow.point.x - window.point.x;
    
    gui::CoordType scaleX = drawingWindow.size.width / window.size.width;
    gui::CoordType scaleY = drawingWindow.size.height / window.size.height;

    gui::CoordType accumShiftX, accumShiftY;
    funkcije[0].getShift(accumShiftX, accumShiftY);
    accumShiftX -= drawingWindow.point.x;
    accumShiftY += drawingWindow.point.y + drawingWindow.size.height;

    for (int i = 0; i < funkcije.size(); ++i){
        funkcije[i].increaseScaleAndShiftX(scaleX, shiftX * scaleX + accumShiftX * (scaleX-1));
        funkcije[i].increaseScaleAndShiftY(scaleY, shiftY * scaleY + accumShiftY * (scaleY-1));
    }

}



void graph::onDraw(const gui::Rect& rect){
    gui::Rect drawingRect({ drawingWindow.point.x, drawingWindow.point.y }, drawingWindow.size);

 
    for (int i = 0; i < funkcije.size(); ++i)
        funkcije[i].draw(drawingRect);

    if (action == Actions::select) 
        gui::Shape::drawRect(selectRect, SELECT_COLOR, 1.5, td::LinePattern::Dash);

    if (drawMargins)
        gui::Shape::drawRect(drawingRect, axisColor, 2);

    drawAxis();

    if (_drawLegend)
        legenda->draw({ rect.right - 20, rect.top + 20 });
    
}




void graph::drawAxis(){
    if (funkcije.size() == 0)
        return;

    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    funkcije[0].getScale(scaleX, scaleY);
    funkcije[0].getShift(shiftX, shiftY);


    auto realToFake = [&scaleX, &shiftX](const gui::CoordType& x) {return x * scaleX + shiftX; };
    auto fakeToReal = [&scaleX, &shiftX](const gui::CoordType& x) {return (x - shiftX) / scaleX; };

    gui::CoordType len = drawingWindow.size.width / scaleX;
    gui::CoordType razmak = 7.27612440527 + std::log2(1 / scaleX); // log2(155) = 7.27612440527 sto prouzrokuje otprilike jednu grid vertikalnu liniju svakih 155 jedinica duzine
    razmak = std::round(razmak);
    razmak = std::pow(2.0, razmak);
    gui::CoordType startVal = std::ceil(fakeToReal(drawingWindow.point.x) / razmak) * razmak;
    gui::CoordType line = realToFake(startVal);





    auto realToFakeY = [&scaleY, &shiftY](const gui::CoordType& x) {return x * scaleY - shiftY; };
    auto fakeToRealY = [&scaleY, &shiftY](const gui::CoordType& x) {return (x + shiftY) / scaleY; };

    gui::CoordType razmakY = 7.27612440527 + std::log2(-1 / scaleY);
    razmakY = std::round(razmakY);
    razmakY = std::pow(2.0, razmakY);
    gui::CoordType startValY = std::ceil(fakeToRealY(drawingWindow.point.y + drawingWindow.size.height) / razmakY) * razmakY;
    gui::CoordType lineY = realToFakeY(startValY);







    const gui::CoordType xAxisHeight = drawingWindow.point.y + drawingWindow.size.height;
    while (line < drawingWindow.point.x + drawingWindow.size.width || lineY >= drawingWindow.point.y){

        constexpr double markLen = 7;
        if (lineY >= drawingWindow.point.y) { // Y osa
            gui::Shape::drawLine({ drawingWindow.point.x - markLen, lineY }, { drawingWindow.point.x + markLen,  lineY }, axisColor, 2);

            gui::DrawableString broj(to_string(startValY));
     
            if (drawMargins) {
                gui::Size sz;
                broj.measure(FONT, sz);
                broj.draw({ drawingWindow.point.x - markLen - 10 - sz.width,  lineY - numberHeight / 2 }, FONT, axisColor);
            }
            else 
                broj.draw({ drawingWindow.point.x + markLen + 5,  lineY }, FONT, axisColor);
            
            if (drawGrid) 
                gui::Shape::drawLine({ drawingWindow.point.x, lineY }, { drawingWindow.point.x + drawingWindow.size.width,  lineY }, axisColor, 1, td::LinePattern::Dash); 

        }

        if (line < drawingWindow.point.x + drawingWindow.size.width) { // X osa
            gui::Shape::drawLine({ line, xAxisHeight - markLen }, { line,  xAxisHeight + markLen }, axisColor, 2); 

            gui::DrawableString broj(to_string(startVal));
            gui::Size sz;
            broj.measure(FONT, sz);

            if (drawMargins) 
                broj.draw({ line - sz.width / 2, xAxisHeight + numberHeight + 5 }, FONT, axisColor);
            else 
                broj.draw({ line - sz.width / 2, xAxisHeight - numberHeight - 12 }, FONT, axisColor);

            if (drawGrid) 
                gui::Shape::drawLine({ line, xAxisHeight }, { line,  drawingWindow.point.y }, axisColor, 1, td::LinePattern::Dash);

        }



        startVal += razmak;
        startValY += razmakY;
        line += razmak * scaleX;
        lineY += razmakY * scaleY;

    }

}


td::String graph::to_string(gui::CoordType x){
    td::String s;
    s.format("%g", x);
    return s;
}




void graph::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){
    action = Actions::select;
    selectRect.setOrigin(inputDevice.getFramePoint());
    selectRect.setWidth(0);
    selectRect.setHeight(0);
}

void graph::onPrimaryButtonReleased(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        action = Actions::none;
        
        gui::Geometry g({ selectRect.left, selectRect.top }, gui::Size(selectRect.width(), selectRect.height()));
        if (selectRect.width() < 0) {
            g.point.x += g.size.width;
            g.size.width *= -1;
        }
        if (selectRect.height() < 0) {
            g.point.y += g.size.height;
            g.size.height *= -1;
        }

        ZoomToWindow(g);

        reDraw();
    }

}

void graph::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
    if (action != Actions::none)
        return;

    action = Actions::secondaryClick;
    lastMousePos = inputDevice.getFramePoint();

}

void graph::onSecondaryButtonReleased(const gui::InputDevice& inputDevice){
    if (action == Actions::drag) {
        action = Actions::none;
        return;
    }
    if (action == Actions::secondaryClick) {
        action = Actions::none;
        Zoom(0.5);
        return;
    }

}





void graph::onCursorMoved(const gui::InputDevice& inputDevice){
    if (action == Actions::secondaryClick) {
        action = Actions::drag;
        return; // moze i bez returna
    }
    if(action == Actions::drag){
        for (int i = 0; i < funkcije.size(); ++i) {
            funkcije[i].increaseShiftX(inputDevice.getFramePoint().x - lastMousePos.x);
            funkcije[i].increaseShiftY(lastMousePos.y - inputDevice.getFramePoint().y);
        }
        lastMousePos = inputDevice.getFramePoint();
        reDraw();
        return;
    }

}

void graph::onCursorDragged(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        selectRect.setWidth(inputDevice.getFramePoint().x - selectRect.left);
        selectRect.setHeight(inputDevice.getFramePoint().y - selectRect.top);
        reDraw();
    }
}

bool graph::onZoom(const gui::InputDevice& inputDevice){
    Zoom((inputDevice.getScale() > 1) ? 1.5 : 0.6666);
    return true;
}

void graph::Zoom(const gui::CoordType &scale){
    if (scale < 0.01)
        return;

  
    gui::CoordType y = drawingWindow.size.height * (1 - 1 / scale) / 2;
    gui::CoordType x = drawingWindow.size.width * (1 - 1 / scale) / 2;

    auto g = drawingWindow;
    g.point.x += x;
    g.point.y += y;
    g.size.height /= scale;
    g.size.width /= scale;

    ZoomToWindow(g);

    reDraw();
}




graph::~graph(){
    delete[] Limits;
    delete legenda;
}




