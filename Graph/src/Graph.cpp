#include "./../inc/Graph.h"
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include "AnnotationDialog.h"
#include "gui/FileDialog.h"
#include "TxtDiaglog.h"
#include"xml/Writer.h"
#include <fstream>
#include "xml/DOMParser.h"
#include "./../../common/ToColor.h"
#include "gui/Transformation.h"

#define SELECT_COLOR td::ColorID::Green
#define FONT gui::Font::ID::SystemBold

const std::initializer_list<gui::InputDevice::Event> Graph::inputs = { gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, 
gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::Keyboard, 
gui::InputDevice::Event::CursorEnterLeave, gui::InputDevice::Event::PrimaryDblClick};

const std::initializer_list<gui::InputDevice::Event> Graph::noInputs = {};


class Graph::legend {
    std::deque<gui::DrawableString> imena;
    std::vector<td::ColorID> colors;
    gui::CoordType length = 0;

    static constexpr double offset = 13;
    static constexpr double rectSize = 21;

public:
    td::ColorID textColor;
    legend(td::ColorID textColor) : textColor(textColor){};

    void draw(const gui::Point& topRight) {
        gui::CoordType height = topRight.y;
        for (size_t i = 0; i < colors.size(); ++i) {
            imena[i].draw({ topRight.x - length, height}, FONT, textColor);
            gui::Shape::drawRect(gui::Rect({ topRight.x - length - offset - rectSize, height }, gui::Size(rectSize, rectSize)), colors[i]);
            height += rectSize + 10;
        }
    }

    void changeName(const td::String& name, size_t poz) {
        imena[poz] = name;
        gui::Size sz;
        imena[poz].measure(FONT, sz);
        if (length < sz.width)
            length = sz.width;
    }
    void changeColor(td::ColorID color, size_t poz) {
        colors[poz] = color;
    }

    void addFunction(const Function& f) {
        imena.resize(imena.size() + 1);
        changeName(*f.name, imena.size() - 1);
        colors.emplace_back(f.getColor());
    };

    gui::CoordType getLength() {
        return length + offset + rectSize;
    }
   
};

td::String Graph::to_string(gui::CoordType x) {
    td::String s;
    s.format("%g", x);
    return s;
}




bool Graph::saveXML(const td::String& path){
    if (_funkcije.size() == 0)
        return true;

    xml::Writer w;
    w.open(path);
    w.startDocument();

    if (!w.isOk())
        return false;

    w.startElement("Model");

    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    _funkcije[0].getScale(scaleX, scaleY);
    _funkcije[0].getShift(shiftX, shiftY);

    w.attribute("minX", to_string(_funkcije[0].transformedToRealX(drawingWindow.point.x)));
    w.attribute("maxX", to_string(_funkcije[0].transformedToRealX(drawingWindow.point.x + drawingWindow.size.width)));
    w.attribute("maxY", to_string(_funkcije[0].TrasformedToRealY(drawingWindow.point.y)));
    w.attribute("minY", to_string(_funkcije[0].TrasformedToRealY(drawingWindow.point.y + drawingWindow.size.height)));

    w.attribute("xLabel", xAxisName.toString());
    w.attribute("yLabel", yAxisName.toString());

    w.attributeC("background", toString(backgroundColor));
    w.attributeC("axis", toString(_axisColor));
    

    for (size_t i = 0; i < _funkcije.size(); ++i){
        size_t length = _funkcije[i].getLength();

        w.startElement("function");
        w.attributeC("name", *_funkcije[i].name);
        w.attribute("width", to_string(_funkcije[i].getLineWidth()));
        w.attributeC("color", toString(_funkcije[i].getColor()));
        w.attributeC("pattern", toString(_funkcije[i].getPattern()));
        w.attribute("points", to_string(length));
        
        
        w.nodeString("x=[");
        const gui::Point* tacke = _funkcije[i].getPoints();
        --length;


        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(_funkcije[i].transformedToRealX(tacke[j].x)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(_funkcije[i].transformedToRealX(tacke[length].x)));
        w.nodeString("]\ny=[");
        for (size_t j = 0; j < length; ++j) {
            w.nodeString(to_string(_funkcije[i].TrasformedToRealY(tacke[j].y)));
            w.nodeString(", ");
        }
        w.nodeString(to_string(_funkcije[i].TrasformedToRealY(tacke[length].y)));
        w.nodeString("]");
        w.endElement();
       
    }


    

        for (auto& an : verticals) {
            w.startNode("annotation");
            w.attribute("x", to_string(an));
            w.endNode();
        }
        for (auto& an : horizontals) {
            w.startNode("annotation");
            w.attribute("y", to_string(an));
            w.endNode();
        }


        

    w.endDocument();
    w.close();

    return true;
}

void Graph::saveTXT(const td::String& path){
    auto d = new txtDiag(this, 10);
    d->openModal([path, this](gui::Dialog* dDlg) {
            saveTXT(path, ((txtDiag*)dDlg)->horizontal);
        });
}



bool Graph::saveTXT(const td::String& path, bool horizontal){
    std::ofstream out;
    out.open(path.c_str());

    if (!out.is_open()) {
        showAlert("Error", "Cant write to file");
        return false;
    }
    
    if (horizontal) {
        for (auto & fun : _funkcije) {
            size_t length = fun.getLength() - 1;
            const gui::Point* tacke = fun.getPoints();

            out << "x=[";
            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.transformedToRealX(tacke[j].x)) << ", ";
            out << to_string(fun.transformedToRealX(tacke[length].x)) << "]\n\n";

            out << fun.name->c_str() << "=[";

            for (size_t j = 0; j < length; ++j)
                out << to_string(fun.TrasformedToRealY(tacke[j].y)) << ", ";
            out << to_string(fun.TrasformedToRealY(tacke[length].y)) << "]\n\n";

        }

    }
    else{

        std::sort(_funkcije.begin(), _funkcije.end(), [](const Function& f1, const Function& f2) {return f1.getLength() > f2.getLength(); });

        size_t length = 0;
        for (auto & fun : _funkcije) {
            out << fun.name->c_str() << "\t\t";
            if (length < fun.getLength())
                length = fun.getLength();
        }
        

        for (size_t i = 0; i < length; ++i){
            out << "\n";
            for (auto & fun : _funkcije) {
                if (fun.getLength() > i) 
                    out << to_string(fun.transformedToRealX(fun.getPoints()[i].x)) << "\t" << to_string(fun.TrasformedToRealY(fun.getPoints()[i].y)) << "\t";
                else 
                    out << "\t\t";
                
            }

        }
           

    }

    return true;
}

bool Graph::openFile(td::String path, bool readOnlyFunctions){
    bool success = false;

    if (path.endsWith(".txt")) {
        readTXT(path);
        success = true;
    }
    if (path.endsWith(".xml")) {
        readXML(path, readOnlyFunctions);
        success = true;
    }

    if (success) {
        _lastPath = path;
        reDraw();
        return true;
    }

    return false;
}

void Graph::readTXT(const td::String& path){
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        showAlert("Error", "Cant open file");
        return;
    }

    std::string line;
    bool containsBracket = false;
    


    while (getline(file, line)) { // pokusava skontati radi li se o horizontalnim podacima ili vertikalnim
        size_t inBracketCharCount = 0;
        size_t outBracketCharCount = 0;
        size_t poz1 = 0;
        if (poz1 = line.find('['), poz1 != std::string::npos) {
            size_t poz2 = line.find(']');
            if (poz2 == std::string::npos) {
                containsBracket = true;
                break;
            }
            inBracketCharCount += poz2 - poz1;
            outBracketCharCount += line.size() - (poz2 - poz1);
            if (inBracketCharCount > outBracketCharCount * 2) {
                containsBracket = true;
                break;
            }
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

                if (name.back() == '=') 
                    name.pop_back();
                

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
                } while (!done && getline(file, line));
                break;
            }

        };

        while (!file.fail() || !file.eof()) {
            std::vector<gui::CoordType> x, y;
            std::string name;
            readData(name, x);
            setxAxisName(name);
            name.clear();
            readData(name, y);
            setyAxisName(name);
            size_t bigger = (x.size() > y.size()) ? y.size() : x.size();
            if (bigger == 0)
                continue;
            this->addFunction(x.data(), y.data(), bigger, 2, td::LinePattern::Solid, name.c_str());
        }

    }
    else {
        // Handle the "%s %s %s....\n%g %g %g %g %g %g\n%g %g %g %g %g %g..." pattern

        auto skipWhiteLine = [&isWhitespace, &line, &file]() {
            while (getline(file, line))
                if (!isWhitespace(line))
                    break;
            return;
            };


        int headerCount = 0;
        skipWhiteLine();
        std::istringstream issHeader(line);
        std::string header;
        std::vector<td::String> names;
        while (issHeader >> header) {
            ++headerCount;
            names.emplace_back(header.c_str());
            header.clear();
        }

        headerCount = headerCount;

        if (headerCount == 0)
            return;

        /*
        char c;
        size_t lines_cnt = 0;
        while (file.get(c)) {
            if (c == '\n')
                ++lines_cnt;
        }
        */

        file.clear();
        file.seekg(0, std::ios::beg);
        skipWhiteLine();
        //skipWhiteLine();
        std::vector<std::vector<gui::Point>> tacke;
   
        try
        {
            tacke.resize(headerCount);
            

            while (getline(file, line)) {
                if (isWhitespace(line))
                    continue;
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
   
}

void Graph::readXML(const td::String& path, bool onlyData){
    xml::FileParser par;
    par.parseFile(path);

    if (!par.isOk()) {
        showAlert("Error", "cant open file");
        return;
    }


    
    auto root = par.getRootNode();

    gui::CoordType minX = 0, maxX = 0, maxY = 0, minY = 0;
    td::ColorID color = td::ColorID::Transparent;

    if (!onlyData) {
        for (auto & att : root->attribs) {

            if (att.getName().cCompareNoCase("xLabel") == 0)
                xAxisName = att.getValue();
            if (att.getName().cCompareNoCase("yLabel") == 0)
                yAxisName = att.getValue();

            if (att.getName().cCompareNoCase("minX") == 0)
                minX = att.value.toDouble();
            if (att.getName().cCompareNoCase("maxX") == 0)
                maxX = att.value.toDouble();
            if (att.getName().cCompareNoCase("minY") == 0)
                maxY = att.value.toDouble();
            if (att.getName().cCompareNoCase("maxY") == 0)
                minY = att.value.toDouble();
            if (att.getName().cCompareNoCase("color") == 0)
                color = to_color(att.getValue().c_str());
            if (att.getName().cCompareNoCase("background") == 0) {
                backgroundColor = to_color(att.getValue().c_str());
                setBackgroundColor(backgroundColor);
            }
            if (att.getName().cCompareNoCase("axis") == 0)
                setAxisColor(to_color(att.getValue().c_str()));
        }



    }



    auto funs = root.getChildNode();

    while (funs.isOk()) {
        if (funs->getName().cCompareNoCase("function") == 0) {
            td::String name = "line";
            double width = 2;
            size_t points = 0;
            td::LinePattern pattern = td::LinePattern::Solid;
            for (auto & att : funs->attribs) {
                if (att.getName().cCompareNoCase("name") == 0)
                    name = att.getValue();
                if (att.getName().cCompareNoCase("width") == 0)
                    width = att.value.toDouble();
                if (att.getName().cCompareNoCase("points") == 0)
                    points = att.value.toInt();
                if (att.getName().cCompareNoCase("pattern") == 0)
                    pattern = td::toLinePattern(att.getValue().c_str());
            }

            

            auto &text = funs->text;

            auto getPoints = [&text, &points](const char* tag, std::vector<gui::CoordType>& vek) {
                int poz = text.find(tag);
                if (poz == -1)
                    return;
                int poz2 = text.find("]", td::UINT4(poz+std::strlen(tag)));
                if (poz2 == -1)
                    return;

                cnt::PushBackVector<td::String> brojevi;
                text.subStr(int(poz + std::strlen(tag)), poz2 - 1).split(",", brojevi, true, true);

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
                
            if (_funkcije.size() == 1 && !onlyData) {
                if (minX == maxX || minY == maxY)
                    fitToWindow();
                else
                    ZoomToArea(&minX, &maxX, &minY, &maxY);
            }

        }

        if (funs->getName().cCompareNoCase("annotation") == 0) {
            auto at = funs.getAttrib("x");
            if (at != nullptr)
                verticals.emplace_back(at->getValue().toDouble());
            at = funs.getAttrib("y");
            if (at != nullptr)
                horizontals.emplace_back(at->getValue().toDouble());
        }

        ++funs;
    }
    reDraw();
}

bool Graph::save(const td::String& path){

    if (path.endsWith(".txt")) {
        saveTXT(path);
        return true;
    }

    if (path.endsWith(".xml")) {
        saveXML(path);
        return true;
    }

   // gui::Geometry g;
   // getGeometry(g);
   // gui::Rect rect({0,0}, gui::Size(g.size.width, g.size.height));

    if (path.endsWith(".png")) {
        gui::Image img(path);
        this->drawToImage(img, 1, false);
        return true;
    }

    if (path.endsWith(".svg")) {
        exportToSVG(path, true);
        return true;
    }

    if (path.endsWith(".pdf")) {
        exportToPDF(path, true);
        return true;
    }

    if (path.endsWith(".eps")) {
        exportToEPS(path, true);
        return true;
    }
    return false;
}

Graph::Graph(bool startWithMargins, bool takeUserInput, td::ColorID backgroundColor) :gui::Canvas(takeUserInput ? inputs : noInputs), backgroundColor(backgroundColor), _drawMargins(startWithMargins)
{
    enableResizeEvent(true);
    registerForScrollEvents();

    for (const char *ime : std::vector<const char*>{":fullScreen", ":grid", ":legend", ":meni", ":save", ":reset", ":fitToWindow", ":info"})
        _slike.emplace_back(ime, gui::Rect({ 0,0 }, gui::Size(32, 32)));
    
    if (backgroundColor == td::ColorID::SysCtrlBack)
        _axisColor = td::ColorID::SysText;
    else if (backgroundColor == td::ColorID::SysText)
        _axisColor = td::ColorID::SysCtrlBack;
    else if (backgroundColor == _axisColor)
        _axisColor = td::ColorID::Black;
    else if (backgroundColor == _axisColor)
        _axisColor = td::ColorID::White;
    setBackgroundColor(backgroundColor);

    gui::DrawableString test("1234567890.1234567890e^(1234567890.1234567890)");
    gui::Size sz;
    test.measure(FONT, sz);
    _numberHeight = sz.height;

    legenda = new legend(_axisColor);


    double distance = 20;
    for (size_t i = 0; i < _slike.size(); ++i) {
        _slike[i].rect.setOrigin(distance, 5);
        distance += 32 + _slike[i].rect.width();
    }

    if (startWithMargins)
        setUpDrawingWindow();
}

void Graph::setUpDrawingWindow(){
    auto past = drawingWindow;
    getGeometry(drawingWindow);
    drawingWindow.point.y = 0;
    drawingWindow.point.x = 0;

    if (_drawMargins) {
        //gui::Point center;
        //center.x = drawingWindow.point.x + drawingWindow.size.width / 2;
        //center.y = drawingWindow.point.y + drawingWindow.size.height / 2;

        drawingWindow.size.width -= _margins.marginLeft + _margins.marginRight;
        drawingWindow.size.height -= _margins.marginTop + _margins.marginBottom;
        drawingWindow.point.x = _margins.marginLeft;
        drawingWindow.point.y = _margins.marginTop;
    }

    ZoomToWindow(past);
    _drawingRect.setOriginAndSize({ drawingWindow.point.x, drawingWindow.point.y }, drawingWindow.size);

}



void Graph::reset(){
    _pastColors.clear();
    verticals.clear();
    horizontals.clear();
    action = Actions::none;
    xAxisName = "";
    yAxisName = "";
    _funkcije.clear();
    delete _Limits;
    _Limits = nullptr;
    delete legenda;
    legenda = new legend(_axisColor);
}

void Graph::setBackgroundColor(td::ColorID color){
    ((gui::Canvas*)this)->setBackgroundColor(color);
    backgroundColor = color;
}

void Graph::setAxisColor(td::ColorID boja){
    _axisColor = boja;
    reDraw();
}

void Graph::setMargins(double top, double left, double right, double bottom)
{
    double _marginTop = top;
    double _marginRight = right;
    double _marginBottom = bottom;
    double _marginLeft = left;
    setUpDrawingWindow();
    reDraw();
}

void Graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, double lineWidth, td::LinePattern pattern, td::String name){
    _funkcije.emplace_back(x, y, length, color, name, lineWidth, pattern);
    finishAddingFunction(_funkcije.back());
}

void Graph::addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth, td::LinePattern pattern, td::String name){
    _funkcije.emplace_back(x, y, length, nextColor(), name, lineWidth, pattern);
    finishAddingFunction(_funkcije.back());
}


void Graph::addFunction(Function&& fun){
    _funkcije.emplace_back(std::move(fun));
    finishAddingFunction(_funkcije.back());
}

void Graph::finishAddingFunction(Function& newFun) {
    _pastColors.push_back(newFun.getColor());
    updateLimits(newFun);
    if (_funkcije.size() == 1) {
        newFun.increaseScaleAndShiftY(-1, 0);
    }
    else {
        gui::CoordType scaleX, scaleY, shiftX, shiftY;
        _funkcije[0].getScale(scaleX, scaleY);
        _funkcije[0].getShift(shiftX, shiftY);
        newFun.increaseScaleAndShiftX(scaleX, shiftX);
        newFun.increaseScaleAndShiftY(scaleY, shiftY);
    }

    legenda->addFunction(newFun);
}

void Graph::updateLimits(const Function& newFun){
    size_t length = newFun.getLength();
    if (length == 0)
        return;

    const gui::Point *tacke = newFun.getPoints();

    if (_Limits == nullptr) {
        _Limits = new gui::CoordType[4];
        _Limits[int(limits::xMin)] = tacke[0].x;
        _Limits[int(limits::xMax)] = tacke[0].x;
        _Limits[int(limits::yMin)] = tacke[0].y;
        _Limits[int(limits::yMax)] = tacke[0].y;
    }

    for (size_t i = 0; i < length; ++i) {
        if (_Limits[int(limits::xMax)] < tacke[i].x)
            _Limits[int(limits::xMax)] = tacke[i].x;

        if (_Limits[int(limits::yMax)] < tacke[i].y)
            _Limits[int(limits::yMax)] = tacke[i].y;

        if (_Limits[int(limits::xMin)] > tacke[i].x)
            _Limits[int(limits::xMin)] = tacke[i].x;

        if (_Limits[int(limits::yMin)] > tacke[i].y)
            _Limits[int(limits::yMin)] = tacke[i].y;
    }
}

void Graph::onResize(const gui::Size& newSize) {
    setUpDrawingWindow();

    if (!_initalDraw)
        fitToWindow();
}

void Graph::fitToWindow(){
    if (_Limits == nullptr)
        return;
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    _initalDraw = true;
    gui::CoordType scaleX, scaleY, shiftX, shiftY;
    _funkcije[0].getScale(scaleX, scaleY);
    _funkcije[0].getShift(shiftX, shiftY);

    gui::Geometry g;
    g.point.x = _Limits[int(limits::xMin)] * scaleX + shiftX;
    g.point.y = _Limits[int(limits::yMax)] * scaleY - shiftY;
    g.size.width = (_Limits[int(limits::xMax)] - _Limits[int(limits::xMin)]) * scaleX;
    g.size.height = (_Limits[int(limits::yMin)] - _Limits[int(limits::yMax)]) * scaleY;


    ZoomToWindow(g);
    reDraw();
}

void Graph::ZoomToWindow(const gui::Geometry& window){
    if (_funkcije.size() == 0)
        return;
    if (window.size.width < 0.1 || window.size.height < 0.1) {
        return;
    }
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    gui::CoordType shiftY = window.point.y + window.size.height - drawingWindow.point.y - drawingWindow.size.height;
    gui::CoordType shiftX = drawingWindow.point.x - window.point.x;
    
    gui::CoordType scaleX = drawingWindow.size.width / window.size.width;
    gui::CoordType scaleY = drawingWindow.size.height / window.size.height;

    gui::CoordType accumShiftX, accumShiftY;
    _funkcije[0].getShift(accumShiftX, accumShiftY);
    accumShiftX -= drawingWindow.point.x;
    accumShiftY += drawingWindow.point.y + drawingWindow.size.height;

    for (int i = 0; i < _funkcije.size(); ++i){
        _funkcije[i].increaseScaleAndShiftX(scaleX, shiftX * scaleX + accumShiftX * (scaleX-1));
        _funkcije[i].increaseScaleAndShiftY(scaleY, shiftY * scaleY + accumShiftY * (scaleY-1));
    }

}


void Graph::ZoomToArea(gui::CoordType* minX, gui::CoordType* maxX, gui::CoordType* minY, gui::CoordType* maxY){
    if (_funkcije.size() == 0)
        return;
    if (drawingWindow.size.width < 1 || drawingWindow.size.height < 1)
        return;

    gui::CoordType left, right, top, bottom;

    left = (minX == nullptr) ? drawingWindow.point.x : _funkcije[0].realToTransformedX(*minX);
    right = (maxX == nullptr) ? (drawingWindow.point.x + drawingWindow.size.width) : _funkcije[0].realToTransformedX(*maxX);

    if (left > right)
        std::swap(left, right);

    bottom = (minY == nullptr) ? (drawingWindow.point.y + drawingWindow.size.height) : _funkcije[0].realToTransformedY(*minY);
    top = (maxY == nullptr) ? drawingWindow.point.y : _funkcije[0].realToTransformedY(*maxY);

    if (top > bottom)
        std::swap(top, bottom);

    

    gui::Geometry g(left, top, right-left, bottom-top);
    ZoomToWindow(g);
    reDraw();

}



void Graph::onDraw(const gui::Rect& rect){

    for (int i = 0; i < _funkcije.size(); ++i)
        _funkcije[i].draw(_drawingRect);

    if (action == Actions::select) 
        gui::Shape::drawRect(_selectRect, SELECT_COLOR, 3, td::LinePattern::Dash);

    if (_drawMargins) {
        gui::Shape::drawRect(_drawingRect, _axisColor, 2);

        for(auto & img : _slike)
            img.image.draw(img.rect);

    }

    drawAxis();


    if (_funkcije.size() == 0)
        return;

    if (_drawLegend)
        legenda->draw({ (_legendLocation.x < 0) ? rect.right + _legendLocation.x : _legendLocation.x, (_legendLocation.y < 0) ? rect.bottom + _legendLocation.x : _legendLocation.y });

    for (size_t i = 0; i < verticals.size(); ++i) {
        gui::CoordType xVal = _funkcije[0].realToTransformedX(verticals[i]);
        if(xVal >= _drawingRect.left && xVal <= _drawingRect.right)
            gui::Shape::drawLine({ xVal, _drawingRect.bottom }, { xVal, _drawingRect.top }, _axisColor, 1.8);
    }
    for (size_t i = 0; i < horizontals.size(); ++i) {
        gui::CoordType yVal = _funkcije[0].realToTransformedY(horizontals[i]);
        if(yVal <= _drawingRect.bottom && yVal >= _drawingRect.top)
            gui::Shape::drawLine({ _drawingRect.left, yVal }, { _drawingRect.right, yVal }, _axisColor, 1.8);
    }
    

    if (action == Actions::pointPeek) {

        td::String broj;
        broj.format("{ %g , %g }", _funkcije[0].transformedToRealX(_lastMousePos.x), _funkcije[0].TrasformedToRealY(_lastMousePos.y));
        gui::DrawableString str(broj);
        gui::Size sz;
        str.measure(FONT, sz);

        gui::Rect pointRect(_lastMousePos, gui::Size(sz.width + 23, sz.height + 23));
        gui::Shape shape;
        shape.createRoundedRect(pointRect, 17.5);
        shape.drawFill(td::ColorID::LightGray);
        pointRect.setTopAndHeight(pointRect.top + 11.5, 0);
        str.draw(pointRect, FONT, td::ColorID::Black, td::TextAlignment::Center, td::VAlignment::Center, td::TextEllipsize::None);
        
    }



}

void Graph::changeWidth(double width, size_t function){
    if (checkRange(function))
        return;

    _funkcije[function].setLineWidth(width);
    reDraw();
}

void Graph::changeName(const td::String& name, size_t function){
    if (checkRange(function))
        return;

    *_funkcije[function].name = name;
    if (legenda == nullptr)
        return;
    legenda->changeName(name, function);
    reDraw();
}

void Graph::changePattern(td::LinePattern pattern, size_t function){
    if (checkRange(function))
        return;

    _funkcije[function].setPattern(pattern);
    reDraw();

}

void Graph::changeColor(td::ColorID color, size_t function){ // todo: modifokovati pastColors da reflektuje promjenu
    if (checkRange(function))
        return;

    _funkcije[function].setColor(color);
    if (legenda == nullptr)
        return;

    legenda->changeColor(color, function);
    reDraw();
}

void Graph::setLegendLocation(const gui::Point& location)
{
    _legendPositionChanged = true;
    _legendLocation = location;
}




td::ColorID Graph::nextColor(){
    td::ColorID boja;
if (_pastColors.size() == 0) {
            
        switch (backgroundColor){
        case td::ColorID::Black:
            boja = td::ColorID::White;
            break;
        case td::ColorID::SysCtrlBack:
            boja = td::ColorID::SysText;
            break;
        case td::ColorID::SysText:
            boja = td::ColorID::SysCtrlBack;
            break;
        default:
            boja = td::ColorID::Black;
        }
    }
    else {
        bool repeat;
        int current = int(_pastColors.back());
        int infiniteLoopCheck = current;
        do {
            repeat = false;
            current += 23;
            current = current % 137; //otprilike sve boje su obuhvacene i svaka boja ce se izabrati prije nego sto se pocnu ponavljati
            for (td::ColorID boja : _pastColors)
                if (current == int(boja) || current == int(backgroundColor)) {
                    repeat = true;
                    break;
                }
            if (infiniteLoopCheck == current)
                _pastColors.clear();
        } while (repeat);
        boja = td::ColorID(current);
    }

    //pastColors.push_back(boja);
    return boja;
}

void Graph::drawAxis(){
    if (_funkcije.size() == 0)
        return;


        gui::Point zero(_funkcije[0].realToTransformedX(0), _funkcije[0].realToTransformedY(0)); //drawing x and y axis
        if (_drawingRect.left <= zero.x && zero.x <= _drawingRect.right)
            gui::Shape::drawLine({ zero.x, _drawingRect.bottom }, { zero.x, _drawingRect.top }, _axisColor, 1.8);
        if (_drawingRect.top <= zero.y && zero.y <= _drawingRect.bottom)
            gui::Shape::drawLine({ _drawingRect.left, zero.y }, { _drawingRect.right, zero.y }, _axisColor, 1.8);
   


    gui::CoordType scaleX, scaleY;
    _funkcije[0].getScale(scaleX, scaleY);

    static double gridLineSpaceX = std::log(7000.0 / gui::Display::getDefaultLogicalPixelToMmVRatio());  //200 mm za svaku grid liniju
    static double gridLineSpaceY = std::log(5000.0 / gui::Display::getDefaultLogicalPixelToMmHRatio());

    gui::CoordType len = drawingWindow.size.width / scaleX;
    gui::CoordType razmak = gridLineSpaceX + std::log2(1 / scaleX);
    razmak = std::round(razmak);
    razmak = std::pow(2.0, razmak);
    gui::CoordType startVal = std::ceil(_funkcije[0].transformedToRealX(drawingWindow.point.x) / razmak) * razmak;
    gui::CoordType line = _funkcije[0].realToTransformedX(startVal);



    gui::CoordType razmakY = gridLineSpaceY + std::log2(-1 / scaleY);
    razmakY = std::round(razmakY);
    razmakY = std::pow(2.0, razmakY);
    gui::CoordType startValY = std::ceil(_funkcije[0].TrasformedToRealY(drawingWindow.point.y + drawingWindow.size.height) / razmakY) * razmakY;
    gui::CoordType lineY = _funkcije[0].realToTransformedY(startValY);





   

    gui::CoordType xAxisHeight;
    gui::CoordType yAxisWidth;

    if(_funkcije[0].TrasformedToRealY(_drawingRect.top) < 0)
        xAxisHeight = _drawingRect.top;
    else if(_funkcije[0].TrasformedToRealY(_drawingRect.bottom) > 0)
        xAxisHeight = _drawingRect.bottom;
    else
        xAxisHeight = _funkcije[0].realToTransformedY(0);

    
    if(_funkcije[0].transformedToRealX(_drawingRect.right) < 0)
        yAxisWidth = _drawingRect.right;
    else if(_funkcije[0].transformedToRealX(_drawingRect.left) > 0)
        yAxisWidth = _drawingRect.left;
    else
        yAxisWidth = _funkcije[0].realToTransformedX(0);
    

    while (line < drawingWindow.point.x + drawingWindow.size.width || lineY >= drawingWindow.point.y){

        constexpr double markLen = 7;
        if (lineY >= drawingWindow.point.y) { // Y osa
            gui::Shape::drawLine({ yAxisWidth - markLen, lineY }, { yAxisWidth + markLen,  lineY }, _axisColor, 2);

            gui::DrawableString broj(to_string(startValY));

            gui::Size sz;
            broj.measure(FONT, sz);

            if ((_drawNumbersOutside && yAxisWidth <= _drawingRect.left + 10) || (!_drawNumbersOutside && yAxisWidth >= _drawingRect.right - sz.width - 10)) {
                broj.draw({ yAxisWidth - markLen - 10 - sz.width,  lineY - _numberHeight / 2 }, FONT, _axisColor);
            }
            else 
                broj.draw({ yAxisWidth + markLen + 5,  lineY}, FONT, _axisColor);
            
            if (_drawGrid) 
                gui::Shape::drawLine({ drawingWindow.point.x, lineY }, { drawingWindow.point.x + drawingWindow.size.width,  lineY }, _axisColor, 1, td::LinePattern::Dash); 

        }

        if (line < drawingWindow.point.x + drawingWindow.size.width) { // X osa
            gui::Shape::drawLine({ line, xAxisHeight - markLen }, { line,  xAxisHeight + markLen }, _axisColor, 2); 

            gui::DrawableString broj(to_string(startVal));
            gui::Size sz;
            broj.measure(FONT, sz);

            if ((_drawNumbersOutside && xAxisHeight >= _drawingRect.top + 10) || ( !_drawNumbersOutside && xAxisHeight <= _drawingRect.top + _numberHeight + 20))
                broj.draw({ line - sz.width / 2, xAxisHeight + _numberHeight + 5 }, FONT, _axisColor);
            else 
                broj.draw({ line - sz.width / 2 + 9, xAxisHeight - _numberHeight - 22 }, FONT, _axisColor);

            if (_drawGrid) 
                gui::Shape::drawLine({ line, _drawingRect.bottom }, { line,  drawingWindow.point.y }, _axisColor, 1, td::LinePattern::Dash);

        }



        startVal += razmak;
        startValY += razmakY;
        line += razmak * scaleX;
        lineY += razmakY * scaleY;

    }


    gui::Rect r({ 0, 0}, gui::Size({ drawingWindow.size.width, 100 }));
    //axis names
    {
        gui::Transformation tr;
           
        constexpr double yAxisNameSeperation = 150;
        constexpr double xAxisNameSeperation = 86;

        tr.saveContext();
        tr.translate(_drawingRect.left, _drawingRect.bottom + xAxisNameSeperation);
        tr.setToContext();
        this->xAxisName.draw(r, FONT, _axisColor, td::TextAlignment::Center);
        tr.restoreContext();
        tr.saveContext();

        r.setWidth(drawingWindow.size.height);
        tr.identity();
        tr.translate(_drawingRect.left - yAxisNameSeperation, _drawingRect.bottom);
        tr.rotateDeg(-90);
        tr.setToContext();
        this->yAxisName.draw(r, FONT, _axisColor, td::TextAlignment::Center);
        tr.restoreContext();
        
    }

}




void Graph::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice){
    if(!_drawingRect.contains(inputDevice.getFramePoint()))
        return;
    action = Actions::pointPeek;
    _lastMousePos = inputDevice.getFramePoint();
    reDraw();
}

bool Graph::onScroll(const gui::InputDevice& inputDevice)
{
    gui::CoordType x = inputDevice.getScrollDelta().x * 20;
    gui::CoordType y = inputDevice.getScrollDelta().y * 20;
    if (inputDevice.getKey().isShiftPressed()) {
        std::swap(x, y);
        x *= -1;
    }

    moveGraph(x,y);
    return true;
}



void Graph::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
    if (_drawingRect.contains(inputDevice.getFramePoint())) {
        action = Actions::select;
        _selectRect.setOrigin(inputDevice.getFramePoint());
        _selectRect.setWidth(0);
        _selectRect.setHeight(0);
        return;
    }

    if (_slike[0].rect.contains(inputDevice.getFramePoint())) {
        if (!_drawNumbersOutside) {
            _drawNumbersOutside = true;
            _margins = _marginsOld;
        }
        else {
            _marginsOld = _margins;
            _margins.marginBottom = 0;
            _margins.marginTop = 47; // slika je visine 32, a odvaja se od vrha 5 tako da 37 ukupno
            _margins.marginLeft = 0;
            if (_legendPositionChanged)
                _margins.marginRight = 0;
            else
                _margins.marginRight = legenda->getLength() + 20 + 10; //legenda se za 20 odvaja od desnog coska po default-u

            _drawNumbersOutside = false;
        }
        setUpDrawingWindow();
        reDraw();
    }

    if (_slike[1].rect.contains(inputDevice.getFramePoint())) {
        showGrid(!_drawGrid);
    }

    if (_slike[2].rect.contains(inputDevice.getFramePoint())) {
        showLegend(!_drawLegend);
    }

    if (_slike[3].rect.contains(inputDevice.getFramePoint())) {
        showInformation();
    }

    if (_slike[4].rect.contains(inputDevice.getFramePoint())) {
        saveMenu();
    }

    if (_slike[5].rect.contains(inputDevice.getFramePoint())) {
        verticals.clear();
        horizontals.clear();
        reDraw();
    }

    if (_slike[6].rect.contains(inputDevice.getFramePoint())) {
        fitToWindow();
    }

    if (_slike[7].rect.contains(inputDevice.getFramePoint())) {
        showHelp();
    }


}

void Graph::showInformation(){
    auto d = new annotDiag(this, verticals, horizontals);
    d->open();
}

void Graph::saveMenu(){
    auto f = new gui::SaveFileDialog(this, "Save plot", { {"Encapsulated PostScript vector graphics format", "*.eps"}, {"Joint Photographic Experts Group image format", "*.jpg"},
        /*{"Portable Network Graphics image format", "*.png"},*/ {"Scalable Vector Graphics image format", "*.svg"},
        {"Plain text file format", "*.txt"}, {"Extensible Markup Language data format", "*.xml"}, {"Portable Document Format document format", "*.pdf"} });
    f->openModal([this](gui::FileDialog* pDlg) {
            return save(pDlg->getFileName());
        });
}

void Graph::onPrimaryButtonReleased(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        action = Actions::none;
        
        gui::Geometry g({ _selectRect.left, _selectRect.top }, gui::Size(_selectRect.width(), _selectRect.height()));
        if (_selectRect.width() < 0) {
            g.point.x += g.size.width;
            g.size.width *= -1;
        }
        if (_selectRect.height() < 0) {
            g.point.y += g.size.height;
            g.size.height *= -1;
        }

        ZoomToWindow(g);

        reDraw();
    }

}

void Graph::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
    if (action == Actions::pointPeek)
        action = Actions::none;
    if (action != Actions::none)
        return;

    action = Actions::secondaryClick;
    _lastMousePos = inputDevice.getFramePoint();

}

void Graph::onSecondaryButtonReleased(const gui::InputDevice& inputDevice){
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


inline void Graph::moveGraph(const gui::CoordType& x, const gui::CoordType& y)
{
    for (int i = 0; i < _funkcije.size(); ++i) {
        _funkcije[i].increaseShiftX(x);
        _funkcije[i].increaseShiftY(y);
    }
    reDraw();
}


void Graph::onCursorMoved(const gui::InputDevice& inputDevice){
    if (action == Actions::secondaryClick) {
        action = Actions::drag;
        return; // moze i bez returna
    }
    if(action == Actions::drag){
        moveGraph(inputDevice.getFramePoint().x - _lastMousePos.x, _lastMousePos.y - inputDevice.getFramePoint().y );
    }
    _lastMousePos = inputDevice.getFramePoint();

}

void Graph::onCursorDragged(const gui::InputDevice& inputDevice){
    if (action == Actions::select) {
        _selectRect.setWidth(inputDevice.getFramePoint().x - _selectRect.left);
        _selectRect.setHeight(inputDevice.getFramePoint().y - _selectRect.top);
        reDraw();
    }
}

bool Graph::onZoom(const gui::InputDevice& inputDevice){
    auto x = (inputDevice.getScale());
    Zoom((x));
    return true;
}

void Graph::Zoom(const gui::CoordType &scale){
    if (scale < 0.0001)
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


bool Graph::onKeyPressed(const gui::Key& key) {
    char c = key.getChar();

    if (c == 'f' || c == 'F') {
        fitToWindow();
        return true;
    }

    if (c == 's' || c == 'S') {
        saveMenu();
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
        showGrid(!_drawGrid);
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F1) {
        showHelp();
        return true;
    }

    if (key.getVirtual() == gui::Key::Virtual::F5) {
        if (_lastPath.isNull())
            return false;
       

        auto funCnt = _funkcije.size();

        delete _Limits;
        _Limits = nullptr;
        delete legenda;
        legenda = new legend(_axisColor);
        xAxisName = "";
        yAxisName = "";
        _pastColors.clear();

        openFile(_lastPath, true);

        _funkcije.erase(_funkcije.begin(), _funkcije.begin() + funCnt);
        reDraw();


        return true;
    }


    if (key.getVirtual() == gui::Key::Virtual::F11) {
        _drawMargins = !_drawMargins;
        setUpDrawingWindow();
        reDraw();
        return true;
    }

    if (_funkcije.size() == 0)
        return false;

    if (c == 'v' || c == 'V' || c == 'h' || c == 'H') {
        if (!gui::Rect({ drawingWindow.point.x, drawingWindow.point.y }, gui::Size(drawingWindow.size.width, drawingWindow.size.height)).contains(_lastMousePos))
            return true;
        if (!_active)
            return false;

        if (c == 'v' || c == 'V')
            verticals.emplace_back(_funkcije[0].transformedToRealX(_lastMousePos.x));
        else
            horizontals.emplace_back(_funkcije[0].TrasformedToRealY(_lastMousePos.y));

        reDraw();
        return true;
    }


    return false;
}

void Graph::showHelp(){

    showAlert("Manual", "F11 - toggle fullscreen\nf - fit to window\nv - add vertical annotaion\nh - add horizontal annotation\ni - show annotations"
        "\ng - toggle grid\n'-' - zoom out\n'+' - zoom in\nL - toggle legend"
        "\nRight click - zoom out\nRight drag (mouse) - move plot\nLeft drag (mouse) - zoom to window\nDouble right click - read point\nF5 - reload from file");

    /*
    showAlert("Uputstvo", "F11 - toggle fullscreen\nf - fit to window\nv - dodaj vertikalnu liniju\nh - dodaj horizontalnu liniju\ni - prikazi informacije o vertiklanim i horizontalnim linijama"
        "\ng - toggle grid\n'-' - zoom out\n'+' - zoom in\nL - toggle legend"
        "\nDesni klik - zoom out\nDesni drag (mis) - pomjeranje grafika\nLijevi drag (mis) - povecavanje na zabiljezeni prozor\nDupli desni klik - ocitanje tacke");
        */
}

void Graph::onCursorExited(const gui::InputDevice& inputDevice) {
    _active = false;

}

void Graph::onCursorEntered(const gui::InputDevice& inputDevice) {
    _active = true;
}


void Graph::measure(gui::CellInfo& cell)
{
    gui::Canvas::measure(cell);
    cell.nResHor = 1;
    cell.nResVer = 1;
}

void Graph::reMeasure(gui::CellInfo& cell)
{
    gui::Canvas::measure(cell);
    cell.nResHor = 1;
    cell.nResVer = 1;
}

Graph::~Graph()
{
    delete[] _Limits;
    delete legenda;
}




