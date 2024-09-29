#include <gui/ToolBar.h>
#include <gui/ToolBarItem.h>
#include <gui/Image.h>
#include <gui/View.h>
#include <gui/HorizontalLayout.h>
#include <gui/NumericEdit.h>
#include <gui/Label.h>

class timesView: public gui::View{
	gui::HorizontalLayout hl;
	gui::NumericEdit startTime, endTime;
	gui::Label _lblStart, _lblEnd;
public:

	timesView():
		hl(4),
		startTime(td::DataType::decimal2),
		endTime(td::DataType::decimal2),
		_lblStart(tr("startTime")),
		_lblEnd(tr("endTime"))
	{
		startTime.setPreferedContentSize({70,13});
		endTime.setPreferedContentSize({70,13});
		endTime.setText("10.00");
		startTime.setText("0.00");
		hl << _lblStart << startTime << _lblEnd << endTime;
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
		addItem(tr("start"), &startIcn, tr("startToolTip"), 100, 0,0, 1);
		addItem("", "daw", &_times);
	}
};


