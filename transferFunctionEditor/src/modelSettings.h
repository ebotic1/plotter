#pragma once
#include "gui/View.h"
#include "property.h"
#include "gui/VerticalLayout.h"

class modelSettings : public gui::View {
	gui::VerticalLayout v;

public:
	elementProperty name;

	modelSettings();


};

