#include "propertySwitcher.h"
#include "globals.h"

properties* globals::switcher = nullptr;
modelSettings* globals::model_settings = nullptr;

properties::properties() :
	gui::ViewSwitcher(settingsCnt+1)
{

	globals::switcher = this;
	globals::model_settings = &modSettings;


	addView(&modSettings, true);
	for(const auto&pok : pogledi)
		addView(pok);

}

void properties::showSettings(BlockBase* block)
{
	if (block == nullptr) {
		showView(0);
		return;
	}
	for (int i = 0; i < settingsCnt; ++i)
		if (pogledi_ID[i] == typeid(*block).hash_code()) {
			block->updateSettingsView(pogledi[i]);
			showView(i + 1);
			return;
		}
		
	int i = 0;
	
	
}
