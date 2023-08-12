#pragma once
#include "gui/Shape.h"

class block {
	gui::Rect _r;
	gui::Shape recShape;

public:
	block(gui::CoordType x = 0, gui::CoordType y = 0, gui::CoordType width = 0, gui::CoordType height = 0);
	const gui::Shape &getShape() const{
		return recShape;
	}
};

block::block(gui::CoordType x, gui::CoordType y, gui::CoordType width, gui::CoordType height) {
	_r.setLeftAndWidth(x, width);
	_r.setTopAndHeight(y, height);
	
	recShape.createRect(_r, 2);
}