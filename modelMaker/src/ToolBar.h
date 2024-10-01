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

class timesView: public gui::View{
	gui::HorizontalLayout hl;
	gui::Label _lblStart, _lblEnd, _lblStep;
public:
	gui::NumericEdit startTime, endTime, stepTime;

	timesView():
		hl(6),
		startTime(td::DataType::decimal2),
		endTime(td::DataType::decimal2),
		stepTime(td::DataType::decimal3),
		_lblStart(tr("startTime")),
		_lblEnd(tr("endTime")),
		_lblStep(tr("stepTime"))
	{
		startTime.setPreferedContentSize({70,13});
		endTime.setPreferedContentSize({70,13});
		stepTime.setPreferedContentSize({70,13});
		endTime.setText("10.00");
		startTime.setText("0.00");
		stepTime.setText("0.01");
		hl << _lblStart << startTime << _lblEnd << endTime << _lblStep << stepTime;
		setLayout(&hl);
	}

};

class ToolBar : public gui::ToolBar {
	gui::Image startIcn;
	timesView _times;
public:
	ToolBar() : 
		gui::ToolBar("main", 2),
		startIcn(":start")
	{
		addItem(tr("start"), &startIcn, tr("startToolTip"), toolBarID, 0,0, td::BYTE(toolBarActionIDs::Simulate));
		addItem("", "ignored", &_times);
	}

	void getStartStopTime(double &startTime, double &endTime, double &stepTime){
		td::Variant start, end;
		_times.startTime.getValue(start);
		_times.endTime.getValue(end);

		startTime = start.dec2Val().getAsFloat();
		endTime = end.dec2Val().getAsFloat();

		_times.stepTime.getValue(end);
		stepTime = end.dec3Val().getAsFloat();
	}
};


