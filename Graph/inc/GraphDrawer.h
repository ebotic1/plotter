#pragma once
#include "./../inc/Function.h"
#include "../src/Legend.h"
#include <gui/Canvas.h>
#include "./../../common/GraphLib.h"
#include <gui/Image.h>
#include <vector>
#include <unordered_set>
#include <gui/DrawableString.h>
#include <gui/Font.h>


class Graph;

class GRAPH_API GraphDrawer
{
    friend class Graph;

    static constexpr td::ColorID SELECT_COLOR = td::ColorID::Green;

	std::vector<Function> _funkcije;
	std::vector<td::ColorID> _pastColors;
	td::ColorID nextColor();
	td::ColorID _axisColor;
    std::unordered_set<td::ColorID> _disallowedColors;
	
	gui::Geometry drawingWindow;
	gui::Rect _drawingRect;
	bool _drawMargins;
	bool _drawNumbersOutside = true;

	struct Margins {
		double marginTop = 20;
		double marginRight = 50;
		double marginBottom = 20;
		double marginLeft = 20;
	}_margins, _marginsZero;


	bool _active = false;
	bool _drawGrid = false;
	gui::DrawableString xAxisName = "", yAxisName = "";
	void drawAxis();
	double _numberHeight;
	td::String to_string(gui::CoordType x);
	inline bool checkRange(const size_t& number) { return number > _funkcije.size(); }

	
	void setUpDrawingWindow();
	void finishAddingFunction(Function& newFun);
	void updateLimits(const Function& newFun);
	void ZoomToWindow(const gui::Geometry& window);
	void moveGraph(const gui::CoordType &x, const gui::CoordType& y);

	enum class limits{xMin = 0, xMax, yMin, yMax};
	gui::CoordType* _Limits = nullptr;
	bool _initalDraw = false;

	

	bool _drawLegend = false;
	bool _legendPositionChanged = false;

	Legend _legenda;


	double _yAxisNameSeperation = 150;
    double _xAxisNameSeperation = 86;

    gui::Font *_font;
    gui::Size _size;
    void onSizeChanged();


protected:




public:

	std::vector<gui::CoordType> verticals, horizontals;
    virtual void drawAgain(){}

    GraphDrawer(td::ColorID axisColor, gui::Font *axisFont, gui::Font *legendFont, bool startWithMargins = false);
    GraphDrawer(const GraphDrawer&) = delete;
    GraphDrawer& operator=(const GraphDrawer&) = delete;
    
    ~GraphDrawer();
	void reset();
	
	void setAxisColor(td::ColorID boja);
	td::ColorID getAxisColor() { return _axisColor; }
	void setxAxisName(const td::String& xName);
	void setyAxisName(const td::String& yName);
	const td::String getxAxisName() { return xAxisName.toString(); }
	const td::String getyAxisName() { return yAxisName.toString(); }
	void setAxisNameDistance(double xNameDistanceFromAxis, double yNameDistanceFromAxis);
	void getAxisNameDistance(double &xNameDistanceFromAxis, double &yNameDistanceFromAxis);

	
	void setMargins(double left, double right, double bottom, double top);
	void getMargins(double &left, double &right, double &bottom, double &top);
	void showLegend(bool draw);
	void showGrid(bool draw) { _drawGrid = draw; drawAgain(); }

	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, double lineWidth = 2, td::LinePattern pattern = td::LinePattern::Solid, td::String name = "line");
	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth = 2, td::LinePattern pattern = td::LinePattern::Solid, td::String name = "line");
	void addFunction(Function && fun);
	
	void fitToWindow();

	

	const std::vector<Function>& getFunctions(){return _funkcije;}
	void changeWidth(double width, size_t function);
	void changeName(const td::String& name, size_t function);
	void changePattern(td::LinePattern pattern, size_t function);
	void changeColor(td::ColorID color, size_t function);
	void setLegendLocation(const gui::Point& location);
	void setLegendCols(int cols);
    void removeColorFromAutopicker(td::ColorID color);
    

	void Zoom(const gui::CoordType& scale);
	void ZoomToArea(gui::CoordType *minX, gui::CoordType *maxX, gui::CoordType *minY, gui::CoordType *maxY);


	void setGraphSize(const gui::Size &sz);
    void draw();

};
