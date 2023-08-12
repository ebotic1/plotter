#pragma once
#include "tBlock.h"
#include "gui/Canvas.h"
#include <vector>

#define BLOCK_COLOR td::ColorID::Black

class kanvas : public gui::Canvas {
	std::vector<block> blocks;
	std::vector<block*> test;
public:
	kanvas();
	void onDraw(const gui::Rect& rect) override;
	inline void createBlock(const gui::Point&p, const gui::Size&s) {
		//blocks.emplace_back(p.x, p.y, s.width, s.height);
		blocks.push_back(block(p.x, p.y, s.width, s.height));
		test.push_back(&blocks.back());
		reDraw();
	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);
	
};

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks }) {

}


void kanvas::onDraw(const gui::Rect& rect){

	for (int i = 0; i < blocks.size(); ++i) {
		blocks[i].getShape().drawFill(td::ColorID::Black);
		//bool x = blocks[i].getShape().isInitialized();
		//int y = 3;
	}

	gui::Shape s;
	
	

}

inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice){
	createBlock(inputDevice.getFramePoint(), {100,100});
}

