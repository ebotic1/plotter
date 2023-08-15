#include "blockSettings.h"
#include "globals.h"

blockSettings::blockSettings() : v(2), num("numerator: ", td::string8, "Numerator of the transfer function (use 's' as a variable)", td::Variant(td::string8, 1)),
	dem("denominator: ", td::string8, "Denominator of the transfer function (use 's' as a variable)")
{
	v << num << dem;
	setLayout(&v);
}

void blockSettings::showBlock(Block* block) {
	globals::switcher->showView(1);
}
