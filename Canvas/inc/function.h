#pragma once
#include "gui/Canvas.h"
#include "./../../common/defineExportImport.h"

class IMPEXP Function {

	td::ColorID color = td::ColorID::Black;
	gui::Point *tacke = nullptr;
	size_t length = 0;
	td::LinePattern pattern = td::LinePattern::Solid;

public:

	Function() {};
	Function(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern);
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

	~Function() {
		delete[] tacke;
	}
};