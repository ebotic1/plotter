#include "modelSettings.h"
#include "blockBase.h"
#include "tBlock.h"
#include "gui/ViewSwitcher.h"
#pragma once

class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	static constexpr int settingsCnt = 1;

	gui::View* pogledi[settingsCnt] = {new BlockBase::settingsView() };
	size_t pogledi_ID[settingsCnt] = {typeid(Block).hash_code() };


public:
	properties();

	~properties() {
		for (auto pok : pogledi)
			delete pok;
	}

	void showSettings(size_t hash_code);

};
