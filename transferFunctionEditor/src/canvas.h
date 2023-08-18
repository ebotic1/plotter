#pragma once
#include "tBlock.h"
#include "gui/Canvas.h"
#include <vector>
#include "globals.h"

#define BLOCK_COLOR td::ColorID::Black
#define BLOCK_COLOR_SELECTED td::ColorID::DeepSkyBlue


class kanvas : public gui::Canvas {
	std::vector<Block *> blocks;
	Block* currentBlock = nullptr;
	gui::Point lastMousePos = { 0,0 };
	double scale = 1;

	enum class Actions{none, wiring, dragging} lastAction = Actions::none;
	Block* tempCurrentBlock = nullptr;

public:
	kanvas();
	void onDraw(const gui::Rect& rect) override;
	inline void createBlock(const gui::Point&p) {
		blocks.push_back(new Block(p));
	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);
	void onCursorDragged(const gui::InputDevice& inputDevice);
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice);
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice);
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice);
	bool onZoom(const gui::InputDevice& inputDevice) override;
	bool getModelSize(gui::Size& size) const override;

	bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI);

	~kanvas();

	friend void globals::refreshCanvas();
	
};

inline bool kanvas::getModelSize(gui::Size& size) const{
	size.height = 0;
	size.width = 0;
	int best = -1;
	for (int i = 0; i < blocks.size(); ++i) {
		if (blocks[i]->getRect().right > size.width)
			best = i;
		if (blocks[i]->getRect().bottom > size.height)
			size.height = blocks[i]->getRect().bottom;

	}

	if (best != -1)
		size.width = blocks[best]->getMostRightPoint().x;

	size.height += 1500;
	size.width += 1500;

	return true;
}

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::PrimaryDblClick, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag }) {
}


void kanvas::onDraw(const gui::Rect& rect){
	
	for (int i = 0; i < blocks.size(); ++i)
		if(blocks[i] != currentBlock)
			blocks[i]->drawBlock(BLOCK_COLOR);
		else
			blocks[i]->drawBlock(BLOCK_COLOR_SELECTED);
	
	if (lastAction == Actions::wiring) {
		gui::Shape::drawLine(tempCurrentBlock->getOutput(), lastMousePos, BLOCK_COLOR, 2, td::LinePattern::Dash);
	}

}

inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){
	this->setFocus(false);

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsBlock(inputDevice.getModelPoint())) {
			lastMousePos = inputDevice.getModelPoint();
			lastAction = Actions::dragging;
			if (blocks[i] == currentBlock)
				return;
			currentBlock = blocks[i];
			globals::block_properties->showBlock(currentBlock);
			reDraw();
			return;
		}

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsOutput(inputDevice.getModelPoint())) {
			tempCurrentBlock = blocks[i];
			lastAction = Actions::wiring;
			return;
		}

	currentBlock = nullptr;
	globals::switcher->showView(0, false);
	reDraw();
	
}

inline void kanvas::onCursorDragged(const gui::InputDevice& inputDevice){
	if (lastAction == Actions::wiring) {
		lastMousePos = inputDevice.getModelPoint();
		reDraw();
		return;
	}
	if (lastAction == Actions::dragging) {
		gui::CoordType deltaX = inputDevice.getModelPoint().x - lastMousePos.x;
		gui::CoordType deltaY = inputDevice.getModelPoint().y - lastMousePos.y;

		currentBlock->setPosition({currentBlock->getRect().left + deltaX, currentBlock->getRect().top + deltaY});
		lastMousePos = inputDevice.getModelPoint();
		return;
	}

}

inline void kanvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice){
	if (lastAction == Actions::wiring) {
		for (int i = 0; i < blocks.size(); ++i)
			if (blocks[i]->intersectsInput(inputDevice.getModelPoint())) {
				tempCurrentBlock->connectTo(blocks[i]);
				break;
			}
		lastAction = Actions::none;
		reDraw();
		return;
	}

	lastAction = Actions::none;
}

inline void kanvas::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice){
	//createBlock(inputDevice.getModelPoint());
}

inline bool kanvas::onZoom(const gui::InputDevice& inputDevice){
	scale *= inputDevice.getScale() > 1 ? 1.5 : 0.66667;
	this->scaleToPoint(scale, inputDevice.getModelPoint());
	return true;
}

inline bool kanvas::onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI){

	if (menuID == 100) {
		if (actionID == 10) {
			createBlock(lastMousePos);
		}
	}

	return true;
}


void kanvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
	lastMousePos = inputDevice.getModelPoint();
	openContextMenu(100, inputDevice);
}



inline kanvas::~kanvas(){
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}