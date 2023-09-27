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

	td::String ulazName, izlazName;
	gui::DrawableString drawUlaz, drawIzlaz;
	gui::Rect inputRect, outputRect;

	bool disableSetUp = false;

public:
	Block(const gui::Point &position, const td::String &inputName, const td::String& outputName);
	const gui::Rect& getRect() const;
	const bool getInputSwitched() const;
	const gui::Point &getOutput() const;
	const gui::Point &getInput() const;
	const gui::Point& getLocation() const;
	const std::vector<Block*>& getConnectedBlocks() const;
	const std::vector<Block*>& getConnectedFromBlocks() const;
	const td::String& getOutputName() const;
	const td::String& getInputName() const;

	void setUpAll();
	void setUpWires(bool refreshCanvas);
	void switchInput();
	void setPosition(const gui::Point &position);
	void removeConnections();

	bool intersectsBlock(const gui::Point &);
	bool intersectsInput(const gui::Point &);
	bool intersectsOutput(const gui::Point&);

	void getAllProps(td::String& nominator, td::String &denominator, bool& connected, bool& switchedInput, td::String &inputName, td::String &outputName);

	void setNominator(const td::String& nominator);
	void setDenominator(const td::String& denominator);
	void setInputName(const td::String& name);
	void setOutputName(const td::String& name);

	void connectTo(Block* block);

	void drawBlock(td::ColorID color);
	void disableLogic(bool disable);

	~Block();

};



