#pragma once
#include "blockSettings.h"
#include "gui/ViewSwitcher.h"
#include "modelSettings.h"
#include "propertySwitcher.h"

class globals {
public:
	static properties* switcher;
	static modelSettings* model_settings;
	static void refreshCanvas();
};
