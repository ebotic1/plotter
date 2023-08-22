#pragma once
#include "./../inc/function.h"
#include "gui/Canvas.h"
#include "./../../common/defineExportImport.h"

#include <vector>


class IMPEXP graph : public gui::Canvas {

	std::vector<Function> funkcije;
	std::vector<td::ColorID> pastColors;
	int lastColor = 0;
	td::ColorID axisColor = td::ColorID::White;
	td::ColorID backgroundColor = td::ColorID::Black;
	static const std::initializer_list<gui::InputDevice::Event> inputs;
	static const std::initializer_list<gui::InputDevice::Event> noInputs;
	gui::Geometry drawingWindow;
	bool drawMargins;
	double marginsFactor = 2;

	bool drawGrid = false;

	enum class Actions { none, select, drag } action = Actions::none;

	void setUpDrawingWindow();

public:


	graph(bool startWithMargins = false, bool takeUserInput = true, td::ColorID backgroundColor = td::ColorID::Black);

	graph(const graph&) = delete;
	graph& operator=(const graph&) = delete;

	void reset();

	void setAxisColor(td::ColorID boja);
	void showMargins(double reductionFactor);

	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern = td::LinePattern::Solid);
	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::LinePattern pattern = td::LinePattern::Solid);
	void addFunction(Function && fun);
	void ZoomToWindow(const gui::Geometry& window);
	void onResize(const gui::Size& newSize) override;

	void onDraw(const gui::Rect& rect);

};