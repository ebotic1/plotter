#pragma once
#include "tBlock.h"
#include "gui/Canvas.h"
#include <vector>
//#include "globals.cpp"

#define BLOCK_COLOR td::ColorID::Black

class kanvas : public gui::Canvas {
	std::vector<Block *> blocks;
public:
	kanvas();
	void onDraw(const gui::Rect& rect) override;
	inline void createBlock(const gui::Point&p, const gui::Size&s) {
		blocks.push_back(new Block(p.x, p.y, s.width, s.height));
		reDraw();
	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice);
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice);

	bool onActionItem(td::BYTE menuID, td::BYTE firstSubMenuID, td::BYTE lastSubMenuID, td::BYTE actionID, gui::ActionItem* pMenuAI) {
		return false;
	}

	~kanvas();

	
	
};

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::PrimaryDblClick }) {

}


void kanvas::onDraw(const gui::Rect& rect){

	for (int i = 0; i < blocks.size(); ++i) 
		blocks[i]->getShape().drawWire(BLOCK_COLOR);

	
	

}

inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){
	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsBlock(inputDevice.getModelPoint())) {
			//globals::block_properties.showBlock(blocks[i]);
		}
	
}

inline void kanvas::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice){
	createBlock(inputDevice.getModelPoint(), { 100,50 });
}


void kanvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice){
	openContextMenu(100, inputDevice);

}



inline kanvas::~kanvas(){
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}