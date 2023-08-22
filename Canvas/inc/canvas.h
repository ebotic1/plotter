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
	bool drawFrame;

	enum class Actions { none, select } action = Actions::none;

public:


	graph(bool startWithFrame = false, bool takeUserInput = true, td::ColorID backgroundColor = td::ColorID::Black);

	graph(const graph&) = delete;
	graph& operator=(const graph&) = delete;

	void reset();

	void setAxisColor(td::ColorID boja);

	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern = td::LinePattern::Solid);
	void addFunction(gui::CoordType* x, gui::CoordType* y, size_t length, td::LinePattern pattern = td::LinePattern::Solid);
	void ZoomToWindow(const gui::Geometry& window);


	void onDraw(const gui::Rect& rect);

};