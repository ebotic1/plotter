#pragma once
#include "blockSettings.h"
#include "gui/ViewSwitcher.h"
#include "modelSettings.h"

class globals {
public:
	static blockSettings* block_properties;
	static gui::ViewSwitcher* switcher;
	static modelSettings* model_settings;
	static void refreshCanvas();
};
