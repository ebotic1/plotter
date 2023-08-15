#pragma once
#include "gui/View.h"
#include "property.h"
#include "gui/VerticalLayout.h"
#include "tBlock.h"

class blockSettings : public gui::View {
	gui::VerticalLayout v;
	elementProperty num, dem;
public:

	blockSettings();
	void showBlock(Block* block);

};


