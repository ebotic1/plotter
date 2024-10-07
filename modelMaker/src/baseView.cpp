#include "baseView.h"
#include <td/Time.h>
#include <cnt/StringBuilder.h>
#include "globalEvents.h"
#include "mainWindow.h"

using LogType = LogView::LogType;

int LogView::errorLen;
int LogView::warLen;
int LogView::infoLen;

LogView::LogView() :
	_vl(1)
{
	textMain.setAsReadOnly();
	_vl << textMain;
	setLayout(&_vl);

	errorLen = tr("error").length();
	warLen = tr("warning").length();
	infoLen = tr("info").length();

}

void LogView::appendLog(const td::String text, LogType type) const
{
	td::Time t(true);
	cnt::StringBuilderSmall str;
	td::ColorID color;

	if (!textMain.isEmpty())
		str << "\n";


	str << "[ " << ((t.getHour() < 10) ? "0" : "\0") << t.getHour() << ':';
	str << ((t.getMinute() < 10) ? "0" : "\0") << t.getMinute() << ':';
	str << ((t.getSecond() < 10) ? "0" : "\0") << t.getSecond() << " ] ";

	gui::Range range(textMain.getText().glyphLength() + str.length(), 0);

	switch (type)
	{
	case LogType::error:
		str << tr("error");
		color = td::ColorID::Crimson;
		range.length = errorLen;
		break;
	case LogType::info:
		str << tr("info");
		range.length = infoLen;
		color = td::ColorID::SysText;
		//color = td::ColorID::RoyalBlue; moze i ova boja ali mozda bolje da ima jedan nivo bez boje
		break;
	case LogType::warning:
		range.length = warLen;
		color = td::ColorID::Gold;
		str << tr("warning");
	}

	str << ": " << text;
	const auto& strFromBuilder = str.toString();
	textMain.appendString(str.toString());


	textMain.setColor(range, color);
}


ViewForTab::ViewForTab(BaseClass* tabView, const td::String &settingsStr):
	tabView(tabView),
	mainView(gui::SplitterLayout::Orientation::Horizontal, gui::SplitterLayout::AuxiliaryCell::First),
	tabAndLogSplitter(gui::SplitterLayout::Orientation::Vertical, gui::SplitterLayout::AuxiliaryCell::Second)
{

	tabAndLogSplitter.setContent(*tabView, logView);
	tabAndLogView.setLayout(&tabAndLogSplitter);
	mainView.setContent(settings, tabAndLogView);

	settings.loadFromString(settingsStr);

	setLayout(&mainView);
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

void ViewForTab::save(){
	if(path.isNull()){
		tabView->saveAs(settings.getString(), &path);
		return;
	}

	if (tabView->save(path, settings.getString()))
		lastSaved = tabView->getVersion();

}

void ViewForTab::saveAs(){
	path.clean();
	tabView->saveAs(settings.getString(), &path);
	lastSaved = tabView->getVersion();
}

void ViewForTab::exportToXML(td::String path)
{
	getModelNode().printNode(path);
}

void ViewForTab::getTimes(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations)
{
	settings.getStartStopTime(startTime, endTime, stepTime, maxIterations);
}

void ViewForTab::setPath(const td::String &path){
	this->path = path;
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
	updateModelNode();
	updateSettings();

	model.clear();
	model = modelTab;

	td::String info("Processing model: ");
	info += name;
	logView.appendLog(info, LogType::info);
	
	for(const auto &dep : depenends)
		if(dep.pathOrTabName != name){
			model.addWtih(GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(dep.pathOrTabName), dep.alias);
		}
	

	return model;
}



ViewForTab::~ViewForTab()
{
	if (lastSaved != tabView->getVersion() || path.isNull()) {
		// reci korisniku da mora servirati
	}
	delete tabView;
}

