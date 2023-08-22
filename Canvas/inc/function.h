#pragma once
#include "gui/Canvas.h"
#include "./../../common/defineExportImport.h"
#include "gui/Shape.h"

class IMPEXP Function {

	td::ColorID color = td::ColorID::Black;
	gui::Point *tacke = nullptr;
	size_t length = 0;
	td::LinePattern pattern = td::LinePattern::Solid;
	//bool recalcShape = true;
	//gui::Shape lines;
	double debljina = 1;

	gui::CoordType shiftX = 0;
	gui::CoordType shiftY = 0;

	gui::CoordType scaleX = 1;
	gui::CoordType scaleY = 1;
	
	gui::Point findIntersection(const gui::Point& p1, const gui::Point& p2, const gui::Rect& r);

public:

	Function() {};
	Function(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern, double lineWidth = 1);
	Function(const Function&) = delete;
	Function(Function&& f) noexcept;
	Function& operator=(Function&& f) noexcept;

	void setPoints(gui::CoordType*x, gui::CoordType*y, size_t lenght);
	void setColor(td::ColorID& color) {
		this->color = color;
	}
	void setPattern(td::LinePattern pattern) {
		this->pattern = pattern;
	}
	void setLineWidth(double width) {
		debljina = width;
	}
	
	void increaseScaleAndShiftX(const gui::CoordType& scale, const gui::CoordType& shift);
	void increaseScaleAndShiftY(const gui::CoordType& scale, const gui::CoordType& shift);

	void increaseShiftX(const gui::CoordType& shift);
	void increaseShiftY(const gui::CoordType& shift);

	void increaseScaleX(const gui::CoordType& scale);
	void increaseScaleY(const gui::CoordType& scale);

	void draw(const gui::Rect& frame);

	~Function() {
		delete[] tacke;
	}
};


