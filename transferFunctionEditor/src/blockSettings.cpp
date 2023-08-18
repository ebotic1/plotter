#include "blockSettings.h"
#include "globals.h"

blockSettings::blockSettings() : v(3), num("numerator: ", td::string8, "Numerator of the transfer function (use 's' as a variable OR use space as an operator)", td::Variant("1")),
	dem("denominator: ", td::string8, "Denominator of the transfer function (use 's' as a variable OR use space as an operator)", td::Variant("s")), inputSwitch("switch input/output")
{
	auto& current = currentBlock;

	num.Action = [&current](td::Variant v) {
		if (current) current->setNominator(v.strVal());
		};
	dem.Action = [&current](td::Variant v) {
		if (current) current->setDenominator(v.strVal());
		};

	v << num << dem;


	v << inputSwitch;



	setLayout(&v);
}

void blockSettings::showBlock(Block* block) {
	globals::switcher->showView(1, false);

	currentBlock = block;
	td::String num, dem;
	bool connected, switched;


	block->getAllProps(num, dem, connected, switched);
	this->num.setValue(num);
	this->dem.setValue(dem);
	inputSwitch.setChecked(switched, false);

}

bool blockSettings::onClick(gui::CheckBox* pBtn){
	currentBlock->switchInput();
	return true;
}

bool blockSettings::onFinishEdit(gui::LineEdit* pCtrl){

	return false;
}
