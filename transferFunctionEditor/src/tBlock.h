#pragma once
#include "gui/Shape.h"

class Block {
	gui::Rect _r;
	gui::Shape recShape;
	bool switchOutput = false;
	gui::Point inputPoint, outputPoint;
	bool connected = false;

	td::String nom, dem;

public:
	Block(gui::CoordType x = 0, gui::CoordType y = 0, gui::CoordType width = 0, gui::CoordType height = 0);
	const gui::Shape& getShape() const;

	void setUpShape();
	void switchInput();

	bool intersectsBlock(const gui::Point &);
	bool intersectsInput(const gui::Point &);
	bool intersectsOutput(const gui::Point&);

	void getAllProps(td::String &nominator, td::String denominator, bool &connected) {

	}
};


