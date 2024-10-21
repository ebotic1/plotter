#pragma once
#include <gui/ToolBar.h>
#include <gui/ToolBarItem.h>
#include <gui/Image.h>
#include <gui/View.h>
#include <gui/HorizontalLayout.h>
#include <gui/NumericEdit.h>
#include <gui/Label.h>

enum class toolBarActionIDs{Simulate = 1};
constexpr int toolBarID = 100;



class ToolBar : public gui::ToolBar {
	gui::Image startIcn;
public:
	ToolBar() : 
		gui::ToolBar("main", 1),
		startIcn(":start")
	{
		addItem(tr("start"), &startIcn, tr("startToolTip"), toolBarID, 0,0, td::BYTE(toolBarActionIDs::Simulate));
	}

};


