#pragma once
#include "gui/View.h"
#include "property.h"
#include "gui/VerticalLayout.h"
#include "tBlock.h"
#include "gui/CheckBox.h"


class blockSettings : public gui::View {
	gui::VerticalLayout v;
	elementProperty num, dem;
	gui::CheckBox inputSwitch;

	Block* currentBlock = nullptr;
public:

	blockSettings();
	void showBlock(Block* block);
	bool onClick(gui::CheckBox* pBtn) override;

	bool onFinishEdit(gui::LineEdit* pCtrl);

};


