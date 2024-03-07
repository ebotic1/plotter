#include "modelSettings.h"
#include "blockBase.h"
#include "tBlock.h"
#include "gui/ViewSwitcher.h"
#pragma once

class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	static constexpr int settingsCnt = 1;

	BlockBase::settingsView* pogledi[settingsCnt] = {new TFBlock::settingsView()};
	size_t pogledi_ID[settingsCnt] = {typeid(TFBlock).hash_code() };


public:
	properties();

	~properties() {
		for (auto pok : pogledi)
			delete pok;
	}

	void showSettings(BlockBase *block);

};
