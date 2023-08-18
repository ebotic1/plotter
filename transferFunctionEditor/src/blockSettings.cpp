#include "blockSettings.h"
#include "globals.h"

blockSettings::blockSettings() : v(7), num("numerator: ", td::string8, "Numerator of the transfer function (use 's' as a variable OR use space as an operator)", td::Variant("1")),
dem("denominator: ", td::string8, "Denominator of the transfer function (use 's' as a variable OR use space as an operator)", td::Variant("s")), 
inputSwitch("switch input/output"), disconnect("disconnect wires", "remove all incomming and outgoing connections from the selected block"),
inputName("input var: ", td::string8, "variable name that will be created when exporting XML file for this block's input"),
outputName("output var: ", td::string8, "variable name that will be created when exporting XML file for this block's output")
{
	auto& current = currentBlock;

	num.Action = [&current](const td::Variant &v) {
		if (current) current->setNominator(v.strVal());
		};
	dem.Action = [&current](const td::Variant& v) {
		if (current) current->setDenominator(v.strVal());
		};

	inputName.Action = [&current](const td::Variant& v) {
		if (current) current->setInputName(v.strVal());
		};

	outputName.Action = [&current](const td::Variant& v) {
		if (current) current->setOutputName(v.strVal());
		};

	v << num << dem;


	v << inputSwitch << disconnect;
	v.appendSpace(5);
	v << inputName << outputName;

	



	setLayout(&v);
}

void blockSettings::showBlock(Block* block) {
	globals::switcher->showView(1, false);

	currentBlock = block;
	td::String num, dem, out, in;
	bool connected, switched;


	block->getAllProps(num, dem, connected, switched, in, out);
	this->num.setValue(num);
	this->dem.setValue(dem);
	inputSwitch.setChecked(switched, false);
	inputName.setValue(in);
	outputName.setValue(out);

}

bool blockSettings::onClick(gui::CheckBox* pBtn){
	currentBlock->switchInput();
	return true;
}

bool blockSettings::onClick(gui::Button* pBtn){
	if (pBtn == &disconnect)
		currentBlock->removeConnections();

	return false;
}

bool blockSettings::onFinishEdit(gui::LineEdit* pCtrl){

	return false;
}
