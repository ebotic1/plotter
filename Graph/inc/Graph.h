#pragma once
#include "./../inc/Function.h"
#include <gui/Canvas.h>
#include "./../../common/GraphLib.h"
#include <gui/Image.h>
#include <vector>
#include <gui/DrawableString.h>
#include <deque>



class GRAPH_API Graph : public gui::Canvas {

	std::vector<Function> _funkcije;
	std::vector<td::ColorID> _pastColors;
	td::ColorID nextColor();
	td::ColorID _axisColor = td::ColorID::White;
	td::ColorID backgroundColor = td::ColorID::Black;
	static const std::initializer_list<gui::InputDevice::Event> inputs;
	static const std::initializer_list<gui::InputDevice::Event> noInputs;
	gui::Geometry drawingWindow;
	gui::Rect _drawingRect;
	bool _drawMargins;
	bool _drawNumbersOutside = true;

	struct Margins {
		double marginTop = 100;
		double marginRight = 50;
		double marginBottom = 50;
		double marginLeft = 100;
	}_margins, _marginsZero;


	bool _active = false;
	bool _drawGrid = false;
	bool _drawButtons = true;
	gui::DrawableString xAxisName = "", yAxisName = "";
	void drawAxis();
	double _numberHeight;
	td::String to_string(gui::CoordType x);
	inline bool checkRange(const size_t& number) { return number > _funkcije.size(); }

	enum class Actions { none, select, secondaryClick, drag, pointPeek, moveLegend } action = Actions::none;

	void setUpDrawingWindow();
	void finishAddingFunction(Function& newFun);
	void updateLimits(const Function& newFun);
	void ZoomToWindow(const gui::Geometry& window);
	void moveGraph(const gui::CoordType &x, const gui::CoordType& y);

	enum class limits{xMin = 0, xMax, yMin, yMax};
	gui::CoordType* _Limits = nullptr;
	bool _initalDraw = false;

	gui::Rect _selectRect;
	gui::Point _lastMousePos;

	bool _drawLegend = false;
	bool _legendPositionChanged = false;

	class legend;
	legend* legenda = nullptr;
	void showInformation();
	void showHelp();
	void saveMenu();
	void readTXT(const td::String& path);
	void readXML(const td::String& path, bool onlyData);
	td::String _txtPut, _lastPath;

	struct imageButton {
		gui::Image image;
		gui::Rect rect;

		imageButton(const gui::Image& image, const gui::Rect& rect) : image(image), rect(rect) {};
	};

	std::vector<imageButton> _slike;

	double _yAxisNameSeperation = 150;
    double _xAxisNameSeperation = 86;


protected:


	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onCursorMoved(const gui::InputDevice& inputDevice) override;
	void onCursorDragged(const gui::InputDevice& inputDevice) override;
	bool onZoom(const gui::InputDevice& inputDevice) override;
	bool onKeyPressed(const gui::Key& key) override;
	void onCursorExited(const gui::InputDevice& inputDevice) override;
	void onCursorEntered(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) override;
	bool onScroll(const gui::InputDevice& inputDevice) override;

	void onDraw(const gui::Rect& rect) override;
	void onResize(const gui::Size& newSize) override;

public:

	std::vector<gui::CoordType> verticals, horizontals;

    Graph(bool startWithMargins = false, bool takeUserInput = true, td::ColorID backgroundColor = td::ColorID::Black);

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    
    ~Graph();

	void reset();

	void setBackgroundColor(td::ColorID color);
	void setAxisColor(td::ColorID boja);
	td::ColorID getAxisColor() { return _axisColor; }
	void setxAxisName(const td::String& xName);
	void setyAxisName(const td::String& yName);
	const td::String getxAxisName() { return xAxisName.toString(); }
	const td::String getyAxisName() { return yAxisName.toString(); }
	void setAxisNameDistance(double xNameDistanceFromAxis, double yNameDistanceFromAxis);
	void getAxisNameDistance(double &xNameDistanceFromAxis, double &yNameDistanceFromAxis);

	td::ColorID getBackgroundColor() { return backgroundColor; }
	void setMargins(double left, double right, double bottom, double top);
	void getMargins(double &left, double &right, double &bottom, double &top);
	void showLegend(bool draw);
	void showGrid(bool draw) { _drawGrid = draw; reDraw(); }

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

	void Zoom(const gui::CoordType& scale);
	void ZoomToArea(gui::CoordType *minX, gui::CoordType *maxX, gui::CoordType *minY, gui::CoordType *maxY);

	bool saveXML(const td::String& path);
	void saveTXT(const td::String& path);
	bool saveTXT(const td::String& path, bool horizontal);
	bool openFile(td::String path, bool readOnlyFunctions = false);

	bool save(const td::String& path);

	void measure(gui::CellInfo& cell) override;
	void reMeasure(gui::CellInfo& cell) override;
	

};
