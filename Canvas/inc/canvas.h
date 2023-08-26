#pragma once
#include "./../inc/function.h"
#include "gui/Canvas.h"
#include "./../../common/defineExportImport.h"
#include "gui/Image.h"
#include <vector>


class IMPEXP graph : public gui::Canvas {

	std::vector<Function> funkcije;
	std::vector<td::ColorID> pastColors;
	int lastColor = 0;
	td::ColorID nextColor();
	td::ColorID axisColor = td::ColorID::White;
	td::ColorID backgroundColor = td::ColorID::Black;
	static const std::initializer_list<gui::InputDevice::Event> inputs;
	static const std::initializer_list<gui::InputDevice::Event> noInputs;
	gui::Geometry drawingWindow;
	gui::Rect drawingRect;
	bool drawMargins;
	double marginsFactor = 2;

	bool active = false;
	bool drawGrid = false;
	void drawAxis();
	double numberHeight;
	td::String to_string(gui::CoordType x);
	inline bool checkRange(const size_t& number) { return number > funkcije.size(); }

	enum class Actions { none, select, secondaryClick, drag, pointPeek } action = Actions::none;

	void setUpDrawingWindow();
	void finishAddingFunction(Function& newFun);
	void updateLimits(const Function& newFun);
	enum class limits{xMin = 0, xMax, yMin, yMax};
	gui::CoordType* Limits = nullptr;
	bool initalDraw = false;

	gui::Rect selectRect;
	gui::Point lastMousePos;

	bool _drawLegend = false;

	class legend;
	legend* legenda = nullptr;
	void showInformation();
	void showHelp();
	void saveMenu();
	td::String txtPut;

	struct imageButton {
		gui::Image image;
		gui::Rect rect;

		imageButton(const gui::Image& image, const gui::Rect& rect) : image(image), rect(rect) {};
	};

	std::vector<imageButton> slike;

public:

	std::vector<gui::CoordType> verticals, horizontals;

	graph(bool startWithMargins = false, bool takeUserInput = true, td::ColorID backgroundColor = td::ColorID::Black);

	graph(const graph&) = delete;
	graph& operator=(const graph&) = delete;

	void reset();

	void setBackgroundColor(td::ColorID color);
	void setAxisColor(td::ColorID boja);
	td::ColorID getAxisColor() { return axisColor; }
	td::ColorID getBackgroundColor() { return backgroundColor; }
	void showMargins(double reductionFactor);
	void showLegend(bool draw) { _drawLegend = draw; reDraw(); }
	void showGrid(bool draw) { drawGrid = draw; reDraw(); }

	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, double lineWidth = 2, td::LinePattern pattern = td::LinePattern::Solid, td::String name = "line");
	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, double lineWidth = 2, td::LinePattern pattern = td::LinePattern::Solid, td::String name = "line");
	void addFunction(Function && fun);
	void ZoomToWindow(const gui::Geometry& window);
	void onResize(const gui::Size& newSize) override;
	void fitToWindow();

	void onDraw(const gui::Rect& rect);

	const std::vector<Function>& getFunctions(){return funkcije;}
	void changeWidth(double width, size_t function);
	void changeName(const td::String& name, size_t function);
	void changePattern(td::LinePattern pattern, size_t function);
	void changeColor(td::ColorID color, size_t function);
	


	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonReleased(const gui::InputDevice & inputDevice) override;
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onCursorMoved(const gui::InputDevice& inputDevice) override;
	void onCursorDragged(const gui::InputDevice& inputDevice) override;
	bool onZoom(const gui::InputDevice& inputDevice) override;
	void Zoom(const gui::CoordType &scale);
	bool onKeyPressed(const gui::Key& key) override;
	void onCursorExited(const gui::InputDevice& inputDevice) override;
	void onCursorEntered(const gui::InputDevice& inputDevice) override;
	bool onClick(gui::Dialog* pDlg, td::UINT4 dlgID) override;
	bool onClick(gui::FileDialog* pDlg, td::UINT4 dlgID) override;
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) override;


	bool saveXML(const td::String& path);
	void saveTXT(const td::String& path);
	bool saveTXT(const td::String& path, bool horizontal);
	void readTXT(const td::String& path);
	void readXML(const td::String& path, bool resetGraph);
	bool save(const td::String& path);

	~graph();

};