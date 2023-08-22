#include "./../inc/canvas.h"



const std::initializer_list<gui::InputDevice::Event> graph::inputs = { gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag,
gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave };

const std::initializer_list<gui::InputDevice::Event> graph::noInputs = {};

graph::graph(bool startWithFrame, bool takeUserInput, td::ColorID backgroundColor) :gui::Canvas(takeUserInput ? inputs : noInputs), backgroundColor(backgroundColor), drawFrame(startWithFrame)
{
    if (backgroundColor == axisColor)
        axisColor = td::ColorID::Black;
    if (backgroundColor == axisColor)
        axisColor = td::ColorID::White;
    setBackgroundColor(backgroundColor);

}

void graph::reset(){
    pastColors.clear();
    lastColor = 0;
    action = Actions::none;
    //funkcije.clear();
}

void graph::setAxisColor(td::ColorID boja){
    axisColor = boja;
    reDraw();
}

void graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern){
    //funkcije.emplace_back(x, y, length, color, pattern);
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

    //funkcije.emplace_back(x, y, length, boja, pattern);
}

void graph::ZoomToWindow(const gui::Geometry& window){

    

}

void graph::onDraw(const gui::Rect& rect){



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
