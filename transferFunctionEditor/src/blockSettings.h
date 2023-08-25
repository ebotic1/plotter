#pragma once
#include "gui/View.h"
#include "./../../common/property.h"
#include "gui/VerticalLayout.h"
#include "tBlock.h"
#include "gui/CheckBox.h"
#include "gui/Button.h"


class blockSettings : public gui::View {
	gui::VerticalLayout v;
	elementProperty num, dem;
	gui::CheckBox inputSwitch;
	gui::Button disconnect;

	elementProperty inputName, outputName;

	Block* currentBlock = nullptr;
protected:
	bool onClick(gui::CheckBox* pBtn) override;
	bool onClick(gui::Button* pBtn) override;
public:

	blockSettings();
	void showBlock(Block* block);

	bool onFinishEdit(gui::LineEdit* pCtrl);

};


