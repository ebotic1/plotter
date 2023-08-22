#include "./../inc/function.h"

Function::Function(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, td::LinePattern pattern) : color(color), pattern(pattern), length(length){
	setPoints(x, y, length);
}


Function::Function(Function&& f) noexcept {
	*this = std::move(f);
}

Function& Function::operator=(Function&& f) noexcept{
	tacke = f.tacke;
	f.tacke = nullptr;
	color = f.color;
	pattern = f.pattern;
	length = f.length;
	return *this;
}

void Function::setPoints(gui::CoordType* x, gui::CoordType* y, size_t length){
	delete[] tacke;
	tacke = new gui::Point[length];

	for (size_t i = 0; i < length; ++i){
		tacke[i].x = x[i];
		tacke[i].y = y[i];
	}

}
