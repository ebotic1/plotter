#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>
#include "./../src/blockBase.h"
#include "squareBlock.h"

#define FONT_ID gui::Font::ID::SystemNormal

class Block: public squareBlockSI, public squareBlockSO, public squareBlock{
	gui::Shape recShape;
	gui::DrawableString drawNom, drawDem;
	gui::Rect rectangleNominator, rectangleDenominator;
	gui::Shape fracLine;

	td::String nom, dem;


	td::String ulazName, izlazName;
	gui::DrawableString drawUlaz, drawIzlaz;
	gui::Rect inputRect, outputRect;



public:




public:
	Block(const gui::Point &position, const td::String &inputName, const td::String& outputName);
	const td::String& getOutputName() const;
	const td::String& getInputName() const;

	void setUpAll() override;

	void setNominator(const td::String& nominator);
	void setDenominator(const td::String& denominator);

	gui::View& getSettingView() {
		return *new gui::View();
	}
	void drawBlock(td::ColorID color);

	~Block();

};

