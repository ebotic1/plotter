#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>

#define FONT_ID gui::Font::ID::ReportBody1

class Block {
	gui::Rect _r;
	gui::Shape recShape;
	bool switchOutput = false;
	gui::Point inputPoint, outputPoint;

	gui::DrawableString drawNom, drawDem;
	gui::Rect rectangleNominator, rectangleDenominator;
	gui::Shape fracLine;

	td::String nom, dem;

	
	std::vector<Block*> connectedTo, connectedFrom;
	std::vector<gui::Shape> connectionLines;

public:
	Block(const gui::Point &position);
	const gui::Rect& getRect() const;
	const bool getInputSwitched() const;
	const gui::Point getMostRightPoint() const;
	const gui::Point &getOutput() const;
	const gui::Point &getInput() const;

	void setUpAll();
	void setUpWires();
	void switchInput();
	void setPosition(const gui::Point &position);

	bool intersectsBlock(const gui::Point &);
	bool intersectsInput(const gui::Point &);
	bool intersectsOutput(const gui::Point&);

	void getAllProps(td::String& nominator, td::String &denominator, bool& connected, bool& switchedInput);

	void setNominator(const td::String& nominator);
	void setDenominator(const td::String& denominator);

	void connectTo(Block* block);

	void drawBlock(td::ColorID color);

};



