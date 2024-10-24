#include "BaseView.h"
#include <td/Time.h>
#include <cnt/StringBuilder.h>
#include "GlobalEvents.h"
#include "MainWindow.h"

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

void LogView::appendLog(const td::String text, LogType type, bool discardThisLog) const
{
	if(discardThisLog)
		return;
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

void LogView::measure(gui::CellInfo &c)
{
	gui::View::measure(c);
	c.minVer = 127;
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
	bool error;
	getModelNode(error, true);
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
		saveAs();
		return;
	}

	if (tabView->save(path, settings.getString())) {
		lastSaved = tabView->getVersion();
		logView.appendLog("Model saved", LogType::info);
	}
	bool error;
	getModelNode(error, true); //samo da se procesuje ime modela i varijable/parametri
}

void ViewForTab::saveAs(){
	bool error;
	getModelNode(error, true);
	path.clean();
	tabView->saveAs(settings.getString(), &path);
	lastSaved = tabView->getVersion(); // u sustini mozda serviranje ne bude uspjesno trebalo bi se u prethodnoj funkciji i pointer na lastSaved poslati ali u 99.99% slucajeva nije bitno, bice servirano
}

void ViewForTab::exportToXML(td::String path)
{
	bool error;
	getModelNode(error).printNode(path);
	if (error)
		showAlert(tr("error"), tr("modelExtractionError"));
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

	tabView->getModel(modelTab);

	lastModelExtract = tabView->getVersion();
}

void ViewForTab::updateSettings()
{
	if (lastSettingsVer == settings.getVersion())
		return;

	lastSettingsVer = settings.getVersion();
	settings.getDependencies(depenends);
	settings.getFunctions(funcionsDesc);

}



const modelNode& ViewForTab::getModelNode(bool &error, bool supressLogs)
{
	error = false;


	const auto& handleInvalidAttribException = [this, &error, supressLogs](modelNode::exceptionInvalidAttribute& atr) {
		cnt::StringBuilderSmall str;
		str << "Invalid attribute \"" << atr.message << "\" on line " << atr.line;
		td::String m;
		str.getString(m);
		error = true;
		logView.appendLog(m, LogType::error, supressLogs);
		return emptyModel;
	};

	const auto& handleInvalidCommandException = [this, &error, supressLogs](modelNode::exceptionInvalidCommand& cmnd) {
		cnt::StringBuilderSmall str;
		str << "Cant process command on line " << cmnd.line << ":\n\t" << cmnd.message;
		td::String m;
		str.getString(m);
		error = true;
		logView.appendLog(m, LogType::error, supressLogs);
		return emptyModel;
		};


	if (includeGuard) {
		logView.appendLog("Circular dependency detected, this model includes itself", LogType::error, supressLogs);
		error = true;
		includeGuard = false;
		return emptyModel;
	}
	else
		includeGuard = true;

	try{
		updateModelNode();
	}catch(modelNode::exceptionInvalidBlockName &blName){
		td::String log("Cant generate model ");
		log += name;
		log += ", unrecognized block ";
		log += blName.message;
		logView.appendLog(log, LogType::error, supressLogs);
		error = true;
		includeGuard = false;
		return emptyModel;
	}
	catch (modelNode::exceptionInvalidAttribute& atr) {
		handleInvalidAttribException(atr);
		return emptyModel;
	}
	catch (modelNode::exceptionInvalidCommand& cmnd) {
		handleInvalidCommandException(cmnd);
		return emptyModel;
	}

	updateSettings();

	model.clear();
	model = modelTab;


	
	for (const auto& dep : depenends) {
		try {
			td::String log;
			if (dep.pathOrTabName.endsWith(".xml"))
				log += "Processing file ";
			else
				log += "Processing model ";


			log += dep.pathOrTabName;
			logView.appendLog(log, LogType::info, supressLogs);
			model.addWtih(GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(dep.pathOrTabName), dep.alias);
		}
		catch (MainWindow::exceptionCantFindTab &) {
			logView.appendLog("Cant find requested model, no tab with such name exists", LogType::error, supressLogs);
			error = true;
		}
		catch (MainWindow::exceptionCantAccessFile &) {
			logView.appendLog("Cant find or access file", LogType::error, supressLogs);
			error = true;
		}
		catch (modelNode::exceptionInvalidBlockName& name) {
			cnt::StringBuilderSmall s;
			s << "Unrecognized block \"" << name.message << "\", Discarding model";
			td::String log;
			s.getString(log);
			logView.appendLog(log, LogType::error, supressLogs);
			error = true;
		}
		catch (modelNode::exceptionInvalidAttribute& atr) {
			handleInvalidAttribException(atr);
			return emptyModel;
		}
		catch (modelNode::exceptionInvalidCommand& cmnd) {
			handleInvalidCommandException(cmnd);
			return emptyModel;
		}
		catch (...) {
			logView.appendLog("Unknown error occured, discarding model", LogType::error, supressLogs);
			error = true;
		}

	}
	
	if (auto it = model._attribs.find("name"); it != model._attribs.end())
		GlobalEvents::getMainWindowPtr()->changeTabName(it->second, this);
	
	std::unordered_set<td::String> vars, params;

	for(const auto &node : model.getNodes()){
		if(std::strcmp(node->getName(), "Params") == 0)
			for(const auto &param : node->getNodes())
				params.insert(param->_attribs["name"]);
		if(std::strcmp(node->getName(), "Vars") == 0)
			for(const auto &param : node->getNodes())
				vars.insert(param->_attribs["name"]);
	}

	tabView->setVariabesAndParams(std::move(vars), std::move(params));

	includeGuard = false;
	return model;
}




ViewForTab::~ViewForTab()
{
	if (lastSaved != tabView->getVersion() || path.isNull()) {
		// reci korisniku da mora servirati
	}
	delete tabView;
}

const std::vector<ModelSettings::FunctionDesc> &ViewForTab::getFunctions()
{
    updateSettings();
	return funcionsDesc;
}

void ViewForTab::BaseClass::setVariabesAndParams(std::unordered_set<td::String>&& vars, std::unordered_set<td::String>&& params)
{
	this->vars = vars;
	this->params = params;
	refreshVisuals();
}

const std::unordered_set<td::String> &ViewForTab::BaseClass::getVars()
{
	return vars;
}

const std::unordered_set<td::String> &ViewForTab::BaseClass::getParams()
{
	return params;
}
