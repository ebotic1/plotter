#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>
#include <gui/View.h>
#include "../../EquationToXML/inc/nodes.h"
#include <set>

#define FONT_ID gui::Font::ID::SystemRegular

class BlockBase {
protected:

	gui::Rect _r;
	bool switchOutput = false;
	std::set<BlockBase*> connectedTo;
	std::vector<std::set<BlockBase *>> connectedFrom;
	gui::Point outputPoint;

	bool disableSetUp = false;


public:
	BlockBase(const gui::Point &position);
	const gui::Rect& getRect() const;
	const bool getInputSwitched() const;
	const gui::Point& getOutput() const;
	virtual const gui::Point& getInput(int poz) const = 0;
	const gui::Point getLocation() const;
	const std::set<BlockBase*>& getConnectedToBlocks() const;
	const std::vector<std::set<BlockBase*>> &getConnectedFromBlocks() const;


	virtual void setUpAll() = 0;
	virtual void setUpWires(bool refreshCanvas) = 0;
	void switchInput();
	void setPosition(const gui::Point &position);
	void removeConnections();

	bool intersectsBlock(const gui::Point &);
	virtual int intersectsInput(const gui::Point&) = 0;
	bool intersectsOutput(const gui::Point&);


	void connectTo(BlockBase* block, int poz);

	virtual void drawBlock(td::ColorID color) = 0;
	void disableLogic(bool disable);

	//virtual gui::View getPropView() = 0;

	//virtual void writeToModel(modelNode& model) = 0;
	//virtual void saveToFile

	virtual ~BlockBase();

};



