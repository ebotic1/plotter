#include "gui/Canvas.h"
#include "canvas.h"

#include "gui/DrawableString.h"
#include "gui/Shape.h"
#include "gui/Transformation.h"
#include <vector>



#define SELECT_COLOR td::ColorID::Black
#define FONT gui::Font::ID::ReportHeader4
#define DEFAULT_BACKGROUND_COLOR td::ColorID::White

void Canvas::updateLimits(const gui::CoordType* x, const gui::CoordType* y, const size_t broj){


    if (funkcije.size() <= 1)
        setLimits(x,y,broj);

    for (size_t i = 0; i < broj; i++)
    {
        if (x[i] < xLim[0]) {
            xLim[0] = x[i];
            pokToMinX = x + i;
        }
        if (x[i] > xLim[1]) xLim[1] = x[i];
        if (y[i] < yLim[0]) {
            yLim[0] = y[i];
            pokToMinY = y + i;
        }
        if (y[i] > yLim[1]) yLim[1] = y[i];
    }
}

void Canvas::setLimits(const gui::CoordType* x, const gui::CoordType* y, const size_t broj){
    
    if (broj > 0) {
        xLim[0] = x[0];
        pokToMinX = x;
        xLim[1] = xLim[1];
        yLim[0] = y[0];
        pokToMinY = y;
        yLim[1] = yLim[0];
    }
    else {
        xLim[0] = -0;
        xLim[1] = 0;
        yLim[0] = 0;
        yLim[1] = 0;
    }

}

void Canvas::dodajFunkciju(const gui::CoordType* x, const gui::CoordType* y, const size_t broj, td::ColorID boja){

    if (broj <= 0) return;

    gui::CoordType* x_p = nullptr;
    gui::CoordType* y_p = nullptr;

    try {
        x_p = new gui::CoordType[broj];
        y_p = new gui::CoordType[broj];
    }
    catch (const std::exception& e) {
        delete[] x_p;
        delete[] y_p;
        throw e;
    }

    std::copy(x, x + broj, x_p);
    std::copy(y, y + broj, y_p);

    funkcije.push_back(kriva{x_p, y_p, boja, broj});

    updateLimits(x_p, y_p, broj);

    for (int i = 0; i < broj; ++i) {
        y_p[i] = -y_p[i] * height_scale + shift_y;
        x_p[i] = x_p[i] * width_scale - shift_x;

    }

}


void Canvas::dodajFunkciju(gui::CoordType*&& x, gui::CoordType*&& y, const size_t broj, td::ColorID boja){

    if (broj <= 0) return;

    gui::CoordType* x_p = x;
    gui::CoordType* y_p = y;

    x = nullptr;
    y = nullptr;

    funkcije.push_back(kriva{x_p, y_p, boja, broj});
    updateLimits(x_p, y_p, broj);

    for (int i = 0; i < broj; ++i) {
        y_p[i] = -y_p[i] * height_scale + shift_y;
        x_p[i] = x_p[i] * width_scale - shift_x;
    }

}


void Canvas::dodajFunkciju(const std::vector<gui::CoordType>& x, const std::vector<gui::CoordType>& y, td::ColorID boja)
{
    dodajFunkciju(x.data(), y.data(), x.size(), boja);
}

void Canvas::dodajFunkciju(std::vector<gui::CoordType>&& x, std::vector<gui::CoordType>&& y, td::ColorID boja){
    provjeriVelicinu(x, y);

    gui::CoordType* xx, *yy;
    xx = x.data();
    yy = y.data();
    dodajFunkciju(xx, yy, x.size(), boja);
    x.clear();
    y.clear();
    
}




Canvas::Canvas(const std::vector<gui::CoordType> &x, const std::vector<gui::CoordType> &y, td::ColorID boja){
    provjeriVelicinu(x, y);
    setLimits(x.data(), y.data(), x.size());
    dodajFunkciju(x.data(), y.data(), x.size());

    drawSetup();
}

Canvas::Canvas(std::vector<gui::CoordType>&& x, std::vector<gui::CoordType>&& y, td::ColorID boja){
    provjeriVelicinu(x, y);
    setLimits(x.data(), y.data(), x.size());
    dodajFunkciju(x.data(), y.data(), x.size());
    x.clear();
    y.clear();
    drawSetup();
}


void Canvas::provjeriVelicinu(const std::vector<gui::CoordType>& x, const std::vector<gui::CoordType>& y){
    if (x.size() != y.size())
        throw std::exception("vektori nemaju istu velicinu");
}

Canvas::~Canvas()
{
    for each (auto & var in funkcije) {
        delete[] var.x;
        delete[] var.y;
    }
    
}

Canvas::Canvas(const gui::CoordType* x, const gui::CoordType* y, const size_t broj) : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave }) {
    setLimits(x, y, broj);
    dodajFunkciju(x, y, broj);

    drawSetup();
}


Canvas::Canvas(gui::CoordType*&& x, gui::CoordType*&& y, const size_t broj) : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag,  gui::InputDevice::Event::CursorMove,gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave }){
   
    gui::CoordType* x_p = x;
    gui::CoordType* y_p = y;

    x = nullptr;
    y = nullptr;

    setLimits(x_p, y_p, broj);
    dodajFunkciju(std::move(x_p), std::move(y_p), broj);

    drawSetup();
}


Canvas::Canvas(): gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag,  gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorEnterLeave }) {
    setLimits(nullptr, nullptr, 0);
    drawSetup();
}

void Canvas::zoomToFit(gui::CoordType limitWidth, gui::CoordType limitHeight){
    gui::Geometry g;
    getGeometry(g);
    g.point.y = 0;
    if(g.size.width * g.size.height == 0)
        g = gui::Geometry(0, 0, initial_width, initial_height - 37);

    gui::CoordType right, top, left, bottom;

    if (limitWidth != 0 && (xLim[1] - xLim[0]) > limitWidth)
        right = limitWidth;
    else
        right = xLim[1] - xLim[0];
    if (limitHeight != 0 && (yLim[1] - yLim[0]) > limitHeight)
        top = limitHeight;
    else 
        top = yLim[1] - yLim[0];
    

    left = *pokToMinX;
    right = left + right * width_scale;
    bottom = *pokToMinY;
    top = bottom - top * height_scale;
   

    setScaleFromWindow(gui::Geometry(left, top, right - left, bottom - top), g);

 
}

void Canvas::drawSetup(){

    gui::Size size;
    gui::DrawableString("1234567890.1234567890").measure(FONT, size);
    font_height = size.height;

    setBackgroundColor(DEFAULT_BACKGROUND_COLOR);

    _select.toZero();

    gui::Geometry g(0, 0, initial_width, initial_height- 37);
    //getGeometry(g);
    setScaleFromWindow(gui::Geometry(xLim[0], -yLim[1], xLim[1] - xLim[0], yLim[1] - yLim[0]), g);

}



bool Canvas::onZoom(const gui::InputDevice& inputDevice){

    Zoom(inputDevice.getScale());

    return true;
}


void Canvas::Zoom(double scale){

    gui::Geometry g;
    getGeometry(g);
    g.point.y = 0;

    if (scale > 1)
        setScaleFromWindow(gui::Geometry(0 + g.size.width / 4, 0 + g.size.height / 4, g.size.width / 2, g.size.height / 2), g);
    else
        setScaleFromWindow(gui::Geometry(0 - g.size.width / 2, 0 - g.size.height / 2, g.size.width * 2, g.size.height * 2), g);

}

void Canvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){

    if (_drag)
        return;
    sel = true;
    auto tacka = inputDevice.getModelPoint();
    _select.toZero();
    _select.top = tacka.y;
    _select.left = tacka.x;

}

void Canvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice){ //popraviti da radi kad pritisnes desni klik
    onCursorDragged(inputDevice);

    gui::Geometry window;
    getGeometry(window);
    gui::Geometry select_window;
  

    if (_select.width() < 0) 
        std::swap(_select.right, _select.left);
    if(_select.height() < 0)
        std::swap(_select.top, _select.bottom);
    
    setScaleFromWindow(gui::Geometry(_select.left, _select.top, _select.width(), _select.height()), window);



    sel = false;
    
}

void Canvas::onCursorDragged(const gui::InputDevice& inputDevice) {
    if (!sel)
        return;
    gui::Point p(inputDevice.getModelPoint());

    _select.setWidth(p.x - _select.left);
    _select.setHeight(p.y - _select.top);


    reDraw();

}


void Canvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
    if (sel)
        return;

    _drag = true;
    _select.toZero();

    auto tacka = inputDevice.getModelPoint();
    _select.setLeftAndWidth(tacka.x, 0);
    _select.setTopAndHeight(tacka.y, 0);
}

void Canvas::onSecondaryButtonReleased(const gui::InputDevice& inputDevice){
    _drag = false;

    if (_select.left == _select.right && _select.top == _select.bottom) {
        Zoom(0.5);
    }

}

void Canvas::onCursorMoved(const gui::InputDevice& inputDevice){
    mis= inputDevice.getModelPoint();

    if (!_drag)
        return;

    auto tacka = inputDevice.getModelPoint();

    gui::CoordType delta_x, delta_y;
    delta_x = _select.left - tacka.x;
    delta_y = _select.top - tacka.y;

    shift_x += delta_x;
    shift_y -= delta_y;

    for each (auto & var in funkcije) {
        gui::CoordType* x = var.x;
        gui::CoordType* y = var.y;

        for (int i = 0; i < var.broj; ++i) {

            x[i] -= delta_x;
            y[i] -= delta_y;


        }
    }


    _select.top = tacka.y;
    _select.left = tacka.x;

    reDraw();
}


void Canvas::setScaleFromWindow(gui::Geometry select, gui::Geometry window){

    //bilo bi dobro da se gui::point moze pomnoziti sa gui::tranformation

    if (select.size.height * select.size.width < 0.01)
        return;

    double hs = window.size.height / select.size.height;
    double ws = window.size.width / select.size.width;

    gui::CoordType local_shiftX = select.point.x;
    gui::CoordType local_shiftY = (-window.size.height + select.point.y + select.size.height);
  

    height_scale *= hs;
    width_scale *= ws;



    const gui::CoordType c1 = *pokToMinX - xLim[0];
    const gui::CoordType c2 = *pokToMinY - yLim[0];

    for each (auto & var in funkcije) {
        gui::CoordType* x = var.x;
        gui::CoordType* y = var.y;

        for (int i = 0; i < var.broj; ++i) {

            x[i] = (x[i] - c1) * ws - (shift_x + select.point.x) * ws;


            y[i] = (y[i] - c2) * hs + window.size.height - (-shift_y + select.point.y + select.size.height) * hs;


        }

    }



    shift_x = (shift_x + select.point.x) * ws;
    shift_y = *pokToMinY;
    


    reDraw();
   
}


bool Canvas::getModelSize(gui::Size& modelSize) const{
    
    modelSize.width = (xLim[1] - xLim[0]) * width_scale;
    modelSize.height = (yLim[1] - yLim[0]) * height_scale;
    return true;
}

void Canvas::onDraw(const gui::Rect& rect){

    gui::Shape graph;

    
    //gui::Shape test;
    //test.drawRect(gui::Rect(375, 365.75, 1125, 1097.25), td::ColorID::BurlyWood); test za zoom, treba da se savrseno poklopi sa kvadratom


    for each (auto & var in funkcije) {
        gui::CoordType* x = var.x;
        gui::CoordType* y = var.y;
        for (int i = 0; i < var.broj-1; ++i) {
            graph.drawLine({ *x, *y }, { *(x+1), *(y+1) }, var.boja, 2);
            ++x;
            ++y;
        }
    }

    
    
    
    constexpr gui::CoordType offset = 20;
    gui::CoordType visina = rect.height() - offset;

    gui::CoordType space = 200. / roundToNearestPowerOf2(width_scale);
    gui::CoordType x_cor, y_cor;

    
    gui::CoordType start = int(shift_x / width_scale / space) * space;
    gui::CoordType x_val = start;

    int uslov = int(rect.width() / (width_scale * abs(space))) + 1;
    //X osa
    for (int i = 0; i < uslov; ++i) {
        x_cor = x_val * width_scale - shift_x;
        gui::DrawableString str(to_string(x_val));
       
        graph.drawLine({ x_cor, visina }, { x_cor, visina + offset }, td::ColorID::Black, 1);
        
        str.draw(gui::Rect(gui::Point{ x_cor - 100, visina - font_height - 5}, gui::Point{ x_cor + 100, visina + 100}), FONT, axisColor, td::TextAlignment::Center);

        x_val += space;
    }



    space = 200. / roundToNearestPowerOf2(height_scale);

    start = int(((-rect.height() + shift_y) / height_scale) / space) * space;
    gui::CoordType y_val = start;

    uslov = int(rect.height() / (height_scale * abs(space))) + 1;
     const gui::CoordType c = *pokToMinY - yLim[0];
    //Y osa
    for (int i = 0; i < uslov; ++i) {
        y_cor = -y_val * height_scale + shift_y;
        gui::DrawableString str(to_string(y_val));

        graph.drawLine({0, y_cor}, {offset, y_cor}, td::ColorID::Black, 1);

        str.draw({offset + 10, y_cor - font_height/2}, FONT, axisColor);

        y_val += space;
    }




    if (sel) {
        graph.drawRect(_select, td::ColorID::Transparent, SELECT_COLOR, 1);
        //select.drawSelectionRect(_select);
    }
    
    for each (auto var in vertikale) {
        auto x = var * width_scale - shift_x;
        graph.drawLine({ x, 0 }, { x, rect.height() }, axisColor, 1);
    }

    for each (auto var in horizontale) {
        auto y = -var * height_scale + shift_y;
        graph.drawLine({0, y}, {rect.width(), y}, axisColor, 1);
    }
    

}


void Canvas::setAxisColor(td::ColorID boja){
    axisColor = boja;
    reDraw();
}


gui::CoordType Canvas::roundToNearestPowerOf2(gui::CoordType number) {
    if (number < 1.0)
        return 1/roundToNearestPowerOf2(1 / number);

        int power = static_cast<int>(std::log2(number));
        gui::CoordType lowerPowerOf2 = std::pow(2.0, power);
        gui::CoordType higherPowerOf2 = lowerPowerOf2 * 2.0;

        if (number - lowerPowerOf2 <= higherPowerOf2 - number)
            return lowerPowerOf2;
        else
            return higherPowerOf2;
    }


bool Canvas::onKeyPressed(const gui::Key& key){

    char c = key.getChar();

    if (c == 'v' || c == 'V'){
        if (!active) return true;
        vertikale.push_back((mis.x + shift_x)/width_scale);
        reDraw();
        return true;
    }

    if (c == 'h' || c == 'H') {
        if (!active) return true;
        horizontale.push_back((-mis.y + shift_y) / height_scale);
        reDraw();
        return true;
    }

    if (c == 'f' ||c == 'F') {
        zoomToFit();
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F1) {
        showAlert("Uputstvo", "f - fit to window\nv - dodaj vertikalnu liniju\nh - dodaj horizontalnu liniju\ni - prikazi informacije o vertiklanim i horizontalnim linijama");
        return true;
    }


       

    if (c== 'i' ||c == 'I') {
        td::String s("vertiklane linije:\n");
        for each (auto var in vertikale){
            s += "x = ";
            s += to_string(var);
            s += "\n";
        }
        s += "\nHorizontalne linije:\n";
        for each (auto var in horizontale) {
            s += "y = ";
            s += to_string(var);
            s += "\n";
        }

        showAlert("informacije o horizontalnim i vertiklanim linijama:", s);
        return true;
    }

    return true;
}


td::String Canvas::to_string(gui::CoordType x){
    td::String s; 
    s.format("%g", x);
    return s;
}

void Canvas::onCursorExited(const gui::InputDevice& inputDevice){
    active = false;

}

void Canvas::onCursorEntered(const gui::InputDevice& inputDevice){
    active = true;
}