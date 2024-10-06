#include "baseView.h"
#include <td/Time.h>
#include <cnt/StringBuilder.h>
#include "globalEvents.h"
#include "mainWindow.h"

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
	mainView(gui::SplitterLayout::Orientation::Horizontal, gui::SplitterLayout::AuxiliaryCell::First),
	tabAndLogView(gui::SplitterLayout::Orientation::Vertical, gui::SplitterLayout::AuxiliaryCell::Second)
{
	//tabAndLogView.setContent(logView, *tabView);
	//mainView.setContent(settings, tabAndLogView);

	auto a = new gui::HorizontalLayout(3);
	*a << settings << logView << *tabView;

	setLayout(a);
}

const LogView &ViewForTab::getLog(){
    return logView;
}

const ViewForTab::BaseClass &ViewForTab::getMainView(){
    return *tabView;
}

const td::String &ViewForTab::getName(){
    return name;
}

void ViewForTab::setName(const td::String &name){
	this->name = name;
}

bool ViewForTab::loadFile(const td::String &path)
{
    td::String settingsString;
	bool retVal = tabView->openFile(path, settingsString);
	if(retVal)
		settings.loadFromString(settingsString);
	return retVal;
}

bool ViewForTab::save(){
	if (tabView->save(path, settings.getString())) {
		lastSaved = tabView->getVersion();
		return true;
	}
	return false;
}

bool ViewForTab::saveAs(){
	td::String newPath = tabView->saveAs(settings.getString());
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
	tabView->getModel(modelTab);
}

void ViewForTab::updateSettings()
{
	if (lastSettingsVer == settings.getVersion())
		return;

	lastSettingsVer = settings.getVersion();
	settings.getDependencies(depenends);
	settings.getFunctions(funcionsDesc);

}

const modelNode& ViewForTab::getModelNode()
{
	if (lastModelExtract == tabView->getVersion() && lastSettingsVer == settings.getVersion())
		return model;

	updateModelNode();
	updateSettings();

	model.clear();
	model = modelTab;

	for(const auto &dep : depenends)
		model.addWtih(GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(dep.pathOrTabName), dep.alias);
	

	return model;
}



ViewForTab::~ViewForTab()
{
	if (lastSaved != tabView->getVersion()) {
		// reci korisniku da mora servirati
	}
	delete tabView;
}

