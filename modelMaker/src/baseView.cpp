#include "baseView.h"
#include <td/Time.h>
#include <cnt/StringBuilder.h>

LogView::LogView() :
	_vl(1)
{
	textMain.setAsReadOnly();
	_vl << textMain;
	setLayout(&_vl);
}

void LogView::appendLog(const td::String text, LogType type) const
{
	td::Time t(true);
	cnt::StringBuilderSmall str;
	td::ColorID color;

	if (!textMain.isEmpty())
		str << "\n";

	gui::Range range(textMain.getText().glyphLength(), 0);

	str << "[ " << ((t.getHour() < 10) ? "0" : "\0") << t.getHour() << ':';
	str << ((t.getMinute() < 10) ? "0" : "\0") << t.getMinute() << ':';
	str << ((t.getSecond() < 10) ? "0" : "\0") << t.getSecond() << " ] ";

	switch (type)
	{
	case LogType::error:
		str << tr("error");
		color = td::ColorID::Crimson;
		break;
	case LogType::info:
		str << tr("info");
		color = td::ColorID::SysText;
		//color = td::ColorID::RoyalBlue; moze i ova boja ali mozda bolje da ima jedan nivo bez boje
		break;
	case LogType::warning:
		color = td::ColorID::Gold;
		str << tr("warning");
	}

	str << ": " << text;
	const auto& strFromBuilder = str.toString();
	range.length = strFromBuilder.glyphLength();
	textMain.appendString(str.toString());


	textMain.setColor(range, color);
}


ViewForTab::ViewForTab(BaseClass* tabView):
	tabView(tabView),
	_hLayout(2),
	mainView(gui::SplitterLayout::Orientation::Vertical, gui::SplitterLayout::AuxiliaryCell::Second)
{
	mainView.setContent(logView, *tabView);
	_hLayout << settings << mainView;

	setLayout(&_hLayout);
}

bool ViewForTab::loadFile(const td::String& path){
	return tabView->openFile(path);
}

bool ViewForTab::save(){
	if (tabView->save(path)) {
		lastSaved = tabView->getVersion();
		return true;
	}
	return false;
}

bool ViewForTab::saveAs(){
	td::String newPath = tabView->saveAs();
	if (!newPath.isNull()) {
		path = newPath;
		lastSaved = tabView->getVersion();
		return true;
	}
	return false;
}

void ViewForTab::exportToXML(td::String path)
{
	getModelNode().printNode(path);
}

void ViewForTab::getTimes(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations)
{
	settings.getStartStopTime(startTime, endTime, stepTime, maxIterations);
}


void ViewForTab::updateModelNode()
{
	if (lastModelExtract == tabView->getVersion())
		return;

	lastModelExtract = tabView->getVersion();
	model.clear();
	tabView->getModel(model);
}

const modelNode& ViewForTab::getModelNode()
{
	updateModelNode();
	return model;
}

inline long long unsigned int ViewForTab::getVersion(){
	tabView->getVersion();
}


ViewForTab::~ViewForTab()
{
	if (lastSaved != tabView->getVersion()) {
		// reci korisniku da mora servirati
	}
	delete tabView;
}

