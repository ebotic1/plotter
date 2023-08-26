#include "./../inc/canvas.h"
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include "annotationDialog.h"
#include "gui/FileDialog.h"
#include "txtDiaglog.h"
#include"xml/Writer.h"
#include <fstream>
#include "xml/DOMParser.h"
#include "./../../common/to_string.h"

#define SELECT_COLOR td::ColorID::Green
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

public:
    td::ColorID textColor;
    legend(td::ColorID textColor) : textColor(textColor){};

    void addFunction(const Function &f) {
        imena.reserve(10);
        imena.emplace_back(f.name->c_str());
        gui::Size sz;
        imena.back().measure(FONT, sz);
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

td::String graph::to_string(gui::CoordType x) {
    td::String s;
    s.format("%g", x);
    return s;
}




bool graph::saveXML(const td::String& path){
    if (funkcije.size() == 0)
        return true;

    xml::Writer w;
    w.open(path);
    w.startDocument();

    if (!w.isOk())
        return false;

    w.startElement("Model");

    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    funkcije[0].getScale(scaleX, scaleY);
    funkcije[0].getShift(shiftX, shiftY);

    w.attribute("scaleX", to_string(scaleX));
    w.attribute("scaleY", to_string(-scaleY));
    w.attribute("shiftX", to_string(shiftX));
    w.attribute("shiftY", to_string(shiftY));

    w.attributeC("background", toString(backgroundColor));
    w.attributeC("axis", toString(axisColor));
    

    for (size_t i = 0; i < funkcije.size(); ++i){
        size_t length = funkcije[i].getLength();

        w.startElement("function");
        w.attributeC("name", *funkcije[i].name);
        w.attribute("width", to_string(funkcije[i].getLineWidth()));
        w.attributeC("color", toString(funkcije[i].getColor()));
        w.attributeC("pattern", toString(funkcije[i].getPattern()));
        w.attribute("points", to_string(length));
        
        
        w.nodeString("x=[");
        const gui::Point* tacke = funkcije[i].getPoints();
        --length;


        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(funkcije[i].transformedToRealX(tacke[j].x)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(funkcije[i].transformedToRealX(tacke[length].x)));
        w.nodeString("]\ny=[");
        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(funkcije[i].TrasformedToRealY(tacke[j].y)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(funkcije[i].TrasformedToRealY(tacke[length].y)));
        w.nodeString("]");
        w.endElement();
       
    }

    w.endDocument();
    w.close();

    return true;
}

void graph::saveTXT(const td::String& path){
    txtPut = path;
    auto d = new txtDiag(this, 10);
    d->openModal(10, this);
}

bool graph::saveTXT(const td::String& path, bool horizontal){
    std::ofstream out;

    out.open(path.c_str());

    if (!out.is_open()) {
        showAlert("Error", "Cant write to file");
        return false;
    }
    
    if (horizontal) {
        for each (auto & fun in funkcije) {
            size_t length = fun.getLength() - 1;
            const gui::Point* tacke = fun.getPoints();

            out << "x=[";
            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.transformedToRealX(tacke[j].x)) << ", ";
            out << to_string(fun.transformedToRealX(tacke[length].x)) << "]\n\n";

            out << fun.name->c_str() << "[";

            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.TrasformedToRealY(tacke[j].y)) << ", ";
            out << to_string(fun.TrasformedToRealY(tacke[length].y)) << "]\n\n";

        }

    }
    else{

        std::sort(funkcije.begin(), funkcije.end(), [](const Function& f1, const Function& f2) {return f1.getLength() > f2.getLength(); });

        size_t length = 0;
        for each (auto & fun in funkcije) {
            out << fun.name->c_str() << "\t\t";
            if (length < fun.getLength())
                length = fun.getLength();
        }
        

        for (size_t i = 0; i < length; ++i){
            out << "\n";
            for each (auto & fun in funkcije) {
                if (fun.getLength() > i) 
                    out << to_string(fun.transformedToRealX(fun.getPoints()[i].x)) << "\t" << to_string(fun.TrasformedToRealY(fun.getPoints()[i].y)) << "\t";
                else 
                    out << "\t\t";
                
            }

        }
           

    }

    return true;
}

void graph::readTXT(const td::String& path){
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        showAlert("Error", "Cant open file");
        return;
    }

    std::string line;
    bool containsBracket = false;


    while (getline(file, line)) {
        if (line.find('[') != std::string::npos) {
            containsBracket = true;
            break;
        }
    }
    
    file.clear();
    file.seekg(0, std::ios::beg);

    auto isWhitespace = [](const std::string& line) {
            return line.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
        };

    if (containsBracket) {
        // Handle the "%s[%g, %g, %g, ....]" pattern

        auto readData = [&file, &line, &isWhitespace](std::string& name, std::vector<gui::CoordType>& values) {
            while (getline(file, line)) {
                if (isWhitespace(line))
                    continue;

                
                std::istringstream iss(line);
                char ch;
                while (iss >> ch) {
                    if (ch == '[')
                        break;
                    name += ch;
                }

                bool done = true;
                do {
                    if (!done)
                        iss.str(line);
                    else
                        done = false;
                       
                    gui::CoordType val;
                    while (iss >> val) {
                        values.emplace_back(val);
                        iss >> ch;
                        if (ch == ']') {
                            done = true;
                            break;
                        }
                    }
                } while (getline(file, line) && !done);
                break;
            }

        };

        while (!file.fail() || !file.eof()) {
            std::vector<gui::CoordType> x, y;
            std::string name;
            readData(name, x);
            name.clear();
            readData(name, y);
            size_t bigger = (x.size() > y.size()) ? y.size() : x.size();
            if (bigger == 0)
                break;
            this->addFunction(x.data(), y.data(), bigger, 2, td::LinePattern::Solid, name.c_str());
        }

    }
    else {
        // Handle the "%s %s %s....\n%g %g %g %g %g %g\n%g %g %g %g %g %g..." pattern

        int headerCount = 0;
        getline(file, line);
        std::istringstream issHeader(line);
        std::string header;
        std::vector<td::String> names;
        while (issHeader >> header) {
            ++headerCount;
            names.emplace_back(header.c_str());
            header.clear();
        }


        char c;
        size_t lines_cnt = 0;
        while (file.get(c)) {
            if (c == '\n')
                ++lines_cnt;
        }

        file.clear();
        file.seekg(0, std::ios::beg);
        getline(file, line);
        std::vector<std::vector<gui::Point>> tacke;
   

        try
        {
            tacke.resize(headerCount);

            while (getline(file, line)) {
                std::istringstream iss(line);

                for (size_t i = 0; i < headerCount; ++i) {
                    gui::CoordType val1, val2;
                    if (iss >> val1 >> val2) 
                        tacke[i].emplace_back(val1, val2);
                }

            }

            for (size_t i = 0; i < headerCount; ++i) {
                addFunction(Function(tacke[i].data(), tacke[i].size(), nextColor(), names[i], 2));
            }
        }
    
        catch (...) {
            showAlert("Error", "No memory");
        }
        
    }
    file.close();
    fitToWindow();
}

void graph::readXML(const td::String& path, bool resetGraph){
    xml::FileParser par;
    par.parseFile(path);

    if (!par.isOk()) {
        showAlert("Error", "cant open file");
        return;
    }


    if (resetGraph)
        this->reset();
    
    auto root = par.getRootNode();

    gui::CoordType scaleX = 0, scaleY = 0, shiftX = 0, shiftY = 0;

    if (resetGraph) {
        for each (auto & att in root->attribs) {
            if (att.getName().cCompareNoCase("scaleX") == 0)
                scaleX = att.value.toDouble();
            if (att.getName().cCompareNoCase("scaleY") == 0)
                scaleY = att.value.toDouble();
            if (att.getName().cCompareNoCase("shiftX") == 0)
                shiftX = att.value.toDouble();
            if (att.getName().cCompareNoCase("shiftY") == 0)
                shiftY = att.value.toDouble();
            if (att.getName().cCompareNoCase("background") == 0) {
                backgroundColor = td::toColorID(att.getName().c_str());
                setBackgroundColor(backgroundColor);
            }
            if (att.getName().cCompareNoCase("axis") == 0)
                setAxisColor(td::toColorID(att.getName().c_str()));
        }



    }



    auto funs = root.getChildNode();

    while (funs.isOk()) {
        if (funs->getName().cCompareNoCase("function") == 0) {
            td::String name = "line";
            double width = 2;
            size_t points = 0;
            td::ColorID color = td::ColorID::Transparent;
            td::LinePattern pattern = td::LinePattern::Solid;
            for each (auto & att in funs->attribs) {
                if (att.getName().cCompareNoCase("name") == 0)
                    name = att.getValue();
                if (att.getName().cCompareNoCase("width") == 0)
                    width = att.value.toDouble();
                if (att.getName().cCompareNoCase("points") == 0)
                    points = att.value.toInt();
                if (att.getName().cCompareNoCase("color") == 0)
                    color = td::toColorID(att.getName().c_str());
                if (att.getName().cCompareNoCase("pattern") == 0)
                    pattern = td::toLinePattern(att.getName().c_str());
            }

            auto &text = funs->text;

            auto getPoints = [&text, &points](const char* tag, std::vector<gui::CoordType>& vek) {
                int poz = text.find(tag);
                if (poz == -1)
                    return;
                int poz2 = text.find("]", poz+std::strlen(tag));
                if (poz2 == -1)
                    return;

                cnt::PushBackVector<td::String> brojevi;
                text.subStr(poz + std::strlen(tag), poz2 - 1).split(",", brojevi, true, true);

                size_t smaller = (points > 0 && points < brojevi.size()) ? points : brojevi.size();

                for (size_t i = 0; i < smaller; ++i) 
                    vek.emplace_back(brojevi[i].toDouble());
                    
                

            };
            
            std::vector<gui::CoordType> x, y;
            getPoints("x=[", x);
            getPoints("y=[", y);

            if (x.size() == 0 || y.size() == 0 || x.size() != y.size())
                continue;

            addFunction(x.data(), y.data(), x.size(), (color == td::ColorID::Transparent) ? nextColor() : color, width, pattern, name);
                
            if (funkcije.size() == 1 && resetGraph) {
                funkcije[0].increaseScaleAndShiftX(scaleX, shiftX);
                funkcije[0].increaseScaleAndShiftY(scaleY, shiftY);
            }

        }

        ++funs;
    }

}

graph::graph(bool startWithMargins, bool takeUserInput, td::ColorID backgroundColor) :gui::Canvas(takeUserInput ? inputs : noInputs), backgroundColor(backgroundColor), drawMargins(startWithMargins)
{
    enableResizeEvent(true);

    for each (const char *ime in std::vector<const char*>{":fullScreen", ":grid", ":legend", ":meni", ":save"})
        slike.emplace_back(ime, gui::Rect({ 0,0 }, gui::Size(32, 32)));
    

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
    auto past = drawingWindow;
    getGeometry(drawingWindow);
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

    ZoomToWindow(past);

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
    delete Limits;
    Limits = nullptr;
    delete legenda;
    legenda = new legend(axisColor);
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
    funkcije.emplace_back(x, y, length, nextColor(), name, lineWidth, pattern);
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

    double distance = 20;
    for (size_t i = 0; i < slike.size(); ++i) {
        slike[i].rect.setOrigin(distance, newSize.height - 32 - slike[i].rect.height());
        distance += 32 + slike[i].rect.width();
    }


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
    reDraw();
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
        gui::Shape::drawRect(selectRect, SELECT_COLOR, 3, td::LinePattern::Dash);

    if (drawMargins) {
        gui::Shape::drawRect(drawingRect, axisColor, 2);

        for each (auto & img in slike)
            img.image.draw(img.rect);

    }

    drawAxis();

    if (funkcije.size() == 0)
        return;

    if (_drawLegend)
        legenda->draw({ rect.right - 20, rect.top + 20 });

    for (size_t i = 0; i < verticals.size(); ++i) {
        gui::CoordType xVal = funkcije[0].realToTransformedX(verticals[i]);
        if(xVal >= drawingRect.left && xVal <= drawingRect.right)
            gui::Shape::drawLine({ xVal, drawingRect.bottom }, { xVal, drawingRect.top }, axisColor, 1.8);
    }
    for (size_t i = 0; i < horizontals.size(); ++i) {
        gui::CoordType yVal = funkcije[0].realToTransformedY(horizontals[i]);
        if(yVal <= drawingRect.bottom && yVal >= drawingRect.top)
            gui::Shape::drawLine({ drawingRect.left, yVal }, { drawingRect.right, yVal }, axisColor, 1.8);
    }
    
}




td::ColorID graph::nextColor(){
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
                if (lastColor == int(boja) || lastColor == int(backgroundColor)) {
                    repeat = true;
                    break;
                }
            if (lastColor == current)
                break;
        } while (repeat);
        boja = td::ColorID(lastColor);
    }

    return boja;
}

void graph::drawAxis(){
    if (funkcije.size() == 0)
        return;

    gui::CoordType scaleX, scaleY;
    funkcije[0].getScale(scaleX, scaleY);


    gui::CoordType len = drawingWindow.size.width / scaleX;
    gui::CoordType razmak = 8.60733031375 + std::log2(1 / scaleX); // log2(390) = 8.60733031375 sto prouzrokuje otprilike jednu grid vertikalnu liniju svakih 390 jedinica duzine
    razmak = std::round(razmak);
    razmak = std::pow(2.0, razmak);
    gui::CoordType startVal = std::ceil(funkcije[0].transformedToRealX(drawingWindow.point.x) / razmak) * razmak;
    gui::CoordType line = funkcije[0].realToTransformedX(startVal);



    gui::CoordType razmakY = 8.60733031375 + std::log2(-1 / scaleY);
    razmakY = std::round(razmakY);
    razmakY = std::pow(2.0, razmakY);
    gui::CoordType startValY = std::ceil(funkcije[0].TrasformedToRealY(drawingWindow.point.y + drawingWindow.size.height) / razmakY) * razmakY;
    gui::CoordType lineY = funkcije[0].realToTransformedY(startValY);







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
                broj.draw({ line - sz.width / 2, xAxisHeight - numberHeight - 22 }, FONT, axisColor);

            if (drawGrid) 
                gui::Shape::drawLine({ line, xAxisHeight }, { line,  drawingWindow.point.y }, axisColor, 1, td::LinePattern::Dash);

        }



        startVal += razmak;
        startValY += razmakY;
        line += razmak * scaleX;
        lineY += razmakY * scaleY;

    }

}







void graph::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
    if (gui::Rect({ drawingWindow.point.x, drawingWindow.point.y }, gui::Size(drawingWindow.size.width, drawingWindow.size.height)).contains(inputDevice.getFramePoint())) {
        action = Actions::select;
        selectRect.setOrigin(inputDevice.getFramePoint());
        selectRect.setWidth(0);
        selectRect.setHeight(0);
        return;
    }

    if (slike[0].rect.contains(inputDevice.getFramePoint())) {
        showMargins(0);
    }

    if (slike[1].rect.contains(inputDevice.getFramePoint())) {
        showGrid(!drawGrid);
    }

    if (slike[2].rect.contains(inputDevice.getFramePoint())) {
        showLegend(!_drawLegend);
    }

    if (slike[3].rect.contains(inputDevice.getFramePoint())) {
        showInformation();
    }

    if (slike[4].rect.contains(inputDevice.getFramePoint())) {
        saveMenu();
    }

    
}

void graph::showInformation(){
    auto d = new annotDiag(this, verticals, horizontals);
    d->open();
}

void graph::saveMenu(){
    auto f = new gui::FileDialog(this, "Save plot", { "*.esp", "*.svg", "*.txt", "*.xml" }, "Save");
    
    //auto f = new gui::SaveFileDialog(this, "d", ".txt,.fsf", "dd", "dawd");
    //auto f = new gui::OpenFileDialog(this, "title", { ".xml" }, "d");
   
    f->openModal(1, this);
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
        reDraw();
    }
    lastMousePos = inputDevice.getFramePoint();

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


bool graph::onKeyPressed(const gui::Key& key) {
    char c = key.getChar();

    if (c == 'f' || c == 'F') {
        fitToWindow();
        return true;
    }

    if (c == 'l' || c == 'L') {
        showLegend(!_drawLegend);
        return true;
    }

    if (c == '+') {
        Zoom(2);
        return true;
    }

    if (c == '-') {
        Zoom(0.5);
        return true;
    }

    if (c == 'i' || c == 'I') {
        showInformation();
        return true;
    }

    if (c == 'g' || c == 'G') {
        showGrid(!drawGrid);
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F1) {
        showAlert("Uputstvo", "f - fit to window\nv - dodaj vertikalnu liniju\nh - dodaj horizontalnu liniju\ni - prikazi informacije o vertiklanim i horizontalnim linijama\nDesni klik - zoom out\nDesni drag (mis) - pomjeranje grafika\nLijevi drag (mis) - povecavanje na zabiljezeni prozor");
        return true;
    }


    if (key.getVirtual() == gui::Key::Virtual::F11) {
        if (drawMargins)
            showMargins(0);
        else
            showMargins(marginsFactor);
        return true;
    }

    if (funkcije.size() == 0)
        return false;

    if (c == 'v' || c == 'V' || c == 'h' || c == 'H') {
        if (!gui::Rect({ drawingWindow.point.x, drawingWindow.point.y }, gui::Size(drawingWindow.size.width, drawingWindow.size.height)).contains(lastMousePos))
            return true;
        if (!active)
            return false;

        if (c == 'v' || c == 'V')
            verticals.emplace_back(funkcije[0].transformedToRealX(lastMousePos.x));
        else
            horizontals.emplace_back(funkcije[0].TrasformedToRealY(lastMousePos.y));

        reDraw();
        return true;
    }


    return false;
}


void graph::onCursorExited(const gui::InputDevice& inputDevice) {
    active = false;

}

void graph::onCursorEntered(const gui::InputDevice& inputDevice) {
    active = true;
}

bool graph::onClick(gui::Dialog* pDlg, td::UINT4 dlgID){

    if (dlgID == 10) {
        saveTXT(txtPut, ((txtDiag*)pDlg)->horizontal);
    }

    reDraw();
    return true;
}

graph::~graph(){
    delete[] Limits;
    delete legenda;
}




