#pragma once
#include "tBlock.h"
#include "gui/Canvas.h"
#include <vector>

#define BLOCK_COLOR td::ColorID::Black

class kanvas : public gui::Canvas {
	std::vector<block *> blocks;
public:
	kanvas();
	void onDraw(const gui::Rect& rect) override;
	inline void createBlock(const gui::Point&p, const gui::Size&s) {
		blocks.push_back(new block(p.x, p.y, s.width, s.height));
		reDraw();
	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);

	~kanvas();

	
	
};

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks }) {

}


void kanvas::onDraw(const gui::Rect& rect){

	for (int i = 0; i < blocks.size(); ++i) 
		blocks[i]->getShape().drawWire(BLOCK_COLOR);

	
	

}

inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){
	createBlock(inputDevice.getFramePoint(), {100,100});
}






inline kanvas::~kanvas(){
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}