#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>
#include "./../src/blockBase.h"
#include <deque>

#define FONT_ID gui::Font::ID::SystemNormal

class Block: public BlockBase{
	gui::Point inputPoint;
	gui::Shape recShape;
	gui::DrawableString drawNom, drawDem;
	gui::Rect rectangleNominator, rectangleDenominator;
	gui::Shape fracLine;

	td::String nom, dem;

	
	
	std::deque<gui::Shape> connectionLines;

	td::String ulazName, izlazName;
	gui::DrawableString drawUlaz, drawIzlaz;
	gui::Rect inputRect, outputRect;



public:




public:
	Block(const gui::Point &position, const td::String &inputName, const td::String& outputName);
	const gui::Point& getInput(int poz) const;
	const td::String& getOutputName() const;
	const td::String& getInputName() const;

	void setUpAll() override;
	void setUpWires(bool refreshCanvas);

	int intersectsInput(const gui::Point &);

	void getAllProps(td::String& nominator, td::String &denominator, bool& connected, bool& switchedInput, td::String &inputName, td::String &outputName);

	void setNominator(const td::String& nominator);
	void setDenominator(const td::String& denominator);
	void setInputName(const td::String& name);
	void setOutputName(const td::String& name);


	void drawBlock(td::ColorID color);

	~Block();

};



