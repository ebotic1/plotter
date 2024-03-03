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
	addView(pogledi[0]);

}

void properties::showSettings(size_t hash_code)
{
	for (int i = 0; i < settingsCnt; ++i)
	{
		if (pogledi_ID[i] == hash_code) {
			showView(i + 1);
			return;
		}
		showView(0);
	}
}
