#include "./../inc/canvas.h"
#include "gui/Shape.h"


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
    drawingWindow.size.height -= drawingWindow.point.y;
    drawingWindow.point.y = 0;

    if (drawMargins) {
        gui::Point center;
        center.x = drawingWindow.point.x + drawingWindow.size.width / 2;
        center.y = drawingWindow.point.y + drawingWindow.size.height / 2;

        drawingWindow.size.width /= marginsFactor;
        drawingWindow.size.height /= marginsFactor;
        drawingWindow.point.x = center.x - drawingWindow.size.height / 2;
        drawingWindow.point.y = center.y - drawingWindow.size.width / 2;
    }

    for (int i = 0; i < funkcije.size(); ++i) {
        funkcije[i].increaseShiftX(drawingWindow.point.x - past.x);
        funkcije[i].increaseShiftY(drawingWindow.point.y - past.y);
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

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern){
    funkcije.emplace_back(x, y, length, color, pattern);
}

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::LinePattern pattern){
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

    funkcije.emplace_back(x, y, length, boja, pattern);

    funkcije.back().increaseScaleY(-1);
    funkcije.back().increaseShiftY(-190);
}

void graph::addFunction(Function&& fun){
    funkcije.emplace_back(std::move(fun));
}

void graph::onResize(const gui::Size& newSize) {
    setUpDrawingWindow();
}

void graph::ZoomToWindow(const gui::Geometry& window){

    gui::CoordType shiftY = window.point.y - drawingWindow.point.y;
    gui::CoordType shiftX = window.point.x - drawingWindow.point.x;
    
    gui::CoordType scaleX = drawingWindow.size.width / window.size.width;
    gui::CoordType scaleY = drawingWindow.size.height / window.size.height;

    for (int i = 0; i < funkcije.size(); ++i){
        funkcije[i].increaseScaleAndShiftX(scaleX, shiftX * scaleX);
        funkcije[i].increaseScaleAndShiftY(scaleY, shiftY * scaleY);
    }

}

#include "gui/Transformation.h"

void graph::onDraw(const gui::Rect& rect){
    gui::Rect drawingRect({ drawingWindow.point.x, drawingWindow.point.y }, drawingWindow.size);

    gui::Transformation t;
    t.translate(400, 400);
    t.scale(0.5);
    t.appendToContext();

    gui::Rect test({ 0,0 }, gui::Size(1000,1000));
    gui::Shape stest;
    stest.createRect(drawingRect);
    stest.drawWire(td::ColorID::Red);

    for (int i = 0; i < funkcije.size(); ++i)
        funkcije[i].draw(drawingRect);



}






















gui::CoordType roundToNearestPowerOf2(gui::CoordType number) {
    if (number < 1.0)
        return 1 / roundToNearestPowerOf2(1 / number);

    int power = static_cast<int>(std::log2(number));
    gui::CoordType lowerPowerOf2 = std::pow(2.0, power);
    gui::CoordType higherPowerOf2 = lowerPowerOf2 * 2.0;

    if (number - lowerPowerOf2 <= higherPowerOf2 - number)
        return lowerPowerOf2;
    else
        return higherPowerOf2;
}
