#include "modelSettings.h"
#include "blockBase.h"
#include "tBlock.h"
#include "gui/ViewSwitcher.h"
#include "sumBlock.h"
#include "nlBlock.h"
#pragma once

class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	static constexpr int settingsCnt = 3;

	BlockBase::settingsView* pogledi[settingsCnt] = {new TFBlock::settingsView(), new sumBlock::settingsView(), new NLBlock::settingsView()};
	size_t pogledi_ID[settingsCnt] = {typeid(TFBlock).hash_code(), typeid(sumBlock).hash_code(), typeid(NLBlock).hash_code()};


public:
	properties();

	~properties() {
		for (auto pok : pogledi)
			delete pok;
	}

	void showSettings(BlockBase *block);

};
