#include "./../inc/canvas.h"
#include "gui/Shape.h"

#define SELECT_COLOR td::ColorID::AliceBlue


const std::initializer_list<gui::InputDevice::Event> graph::inputs = { gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag,
gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave };

const std::initializer_list<gui::InputDevice::Event> graph::noInputs = {};

graph::graph(bool startWithMargins, bool takeUserInput, td::ColorID backgroundColor) :gui::Canvas(takeUserInput ? inputs : noInputs), backgroundColor(backgroundColor), drawMargins(startWithMargins)
{

    enableResizeEvent(true);

    if (backgroundColor == axisColor)
        axisColor = td::ColorID::Black;
    if (backgroundColor == axisColor)
        axisColor = td::ColorID::White;
    setBackgroundColor(backgroundColor);

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

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, double lineWidth, td::LinePattern pattern){
    funkcije.emplace_back(x, y, length, color, lineWidth, pattern);
    finishAddingFunction(funkcije.back());
}

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth, td::LinePattern pattern){
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

    funkcije.emplace_back(x, y, length, boja, lineWidth, pattern);
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


gui::CoordType roundToNearestPowerOf2(gui::CoordType number) {
    if (number < 1.0)
        return 1 / roundToNearestPowerOf2(1 / number);

    int power = static_cast<int>(std::log2(number));
    gui::CoordType lowerPowerOf2 = std::pow(2.0, power);
    gui::CoordType higherPowerOf2 = lowerPowerOf2 * 2.0;

    //return higherPowerOf2;

    if (number - lowerPowerOf2 <= higherPowerOf2 - number)
        return lowerPowerOf2;
    else
        return higherPowerOf2;
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
    

}

void graph::drawAxis(){
    if (funkcije.size() == 0)
        return;

    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    funkcije[0].getScale(scaleX, scaleY);
    funkcije[0].getShift(shiftX, shiftY);


    auto realToFake = [&scaleX, shiftX](const gui::CoordType& x) {return x * scaleX + shiftX; };
    auto fakeToReal = [&scaleX, shiftX](const gui::CoordType& x) {return (x - shiftX) / scaleX; };

    shiftX -= drawingWindow.point.x;
    shiftY += drawingWindow.point.y + drawingWindow.size.height;

    gui::CoordType len = drawingWindow.size.width / scaleX;

    gui::CoordType razmak = int(std::log2(160) + std::log2(1 / scaleX)) + 1; // log2(200) = 7.64385618977 sto je za jednu linija svakih 200 jedinica duzine
    razmak = std::pow(2.0, razmak);


    gui::CoordType startVal = (0 + int(fakeToReal(drawingWindow.point.x) / razmak)) * razmak;
    gui::CoordType line = realToFake(startVal);

    

    while (line < drawingWindow.point.x + drawingWindow.size.width) {
    
        gui::Shape::drawLine({ line, 0 }, { line, 1500 }, td::ColorID::Red, 2);

        line += razmak * scaleX;

    }

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
    if (scale == 0)
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
}






















