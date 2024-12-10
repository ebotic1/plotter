#pragma once
#include "modelSettings.h"
#include "blocks/blockBase.h"
#include "blocks/tBlock.h"
#include <gui/ViewSwitcher.h>
#include "blocks/sumBlock.h"
#include "blocks/nlBlock.h"
#include "blocks/ProductBlock.h"


class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	static constexpr int settingsCnt = 4;

	BlockBase::settingsView* pogledi[settingsCnt] = {new TFBlock::settingsView(), new sumBlock::settingsView(), new NLBlock::settingsView(), new ProductBlock::settingsView()};
	size_t pogledi_ID[settingsCnt] = {typeid(TFBlock).hash_code(), typeid(sumBlock).hash_code(), typeid(NLBlock).hash_code(), typeid(ProductBlock).hash_code()};


public:
	properties();

	~properties() {
		for (auto pok : pogledi)
			delete pok;
	}

	modelSettings* getModelSettings();
	void showSettings(BlockBase *block);

};
