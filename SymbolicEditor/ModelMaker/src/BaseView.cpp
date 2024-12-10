#include "BaseView.h"
#include <td/Time.h>
#include <cnt/StringBuilder.h>
#include "GlobalEvents.h"
#include "MainWindow.h"
#include "SaveDlg.h"

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
	case LogType::Error:
		str << tr("error");
		color = td::ColorID::Crimson;
		range.length = errorLen;
		break;
	case LogType::Info:
		str << tr("info");
		range.length = infoLen;
		color = td::ColorID::SysText;
		//color = td::ColorID::RoyalBlue; moze i ova boja ali mozda bolje da ima jedan nivo bez boje
		break;
	case LogType::Warning:
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

ViewForTab::ViewForTab(BaseClass* tabView, const td::String &settingsStr)
: tabView(tabView)
, _logImg(":txtIcon")
, _settingsImg(":settings")
, _resultsImg(":results")
, tabAndLogSplitter(gui::SplitterLayout::Orientation::Vertical, gui::SplitterLayout::AuxiliaryCell::Second)
, logView(new LogView)  //IDz: Why new LogView??
{

	_tabView.addView(logView.get(), tr("log") , &_logImg);
	_tabView.addView(&settings, tr("attributes") , &_settingsImg);
    _tabView.addView(&_results, tr("Results"), &_resultsImg);
	tabAndLogSplitter.setContent(*tabView, _tabView);

	settings.loadFromString(settingsStr);
	_lastSavedSettings = settings.getVersion();
	_lastSavedModel = tabView->getVersion();
	bool error;
	getModelNode(error, true);

	setLayout(&tabAndLogSplitter);
}

const std::shared_ptr<LogView> ViewForTab::getLog()
{
    return logView;
}

Results* ViewForTab::getResults()
{
    return &_results;
}

const ViewForTab::BaseClass &ViewForTab::getMainView()
{
    return *tabView;
}

ViewForTab::BaseClass* ViewForTab::getContentView()
{
    return tabView;
}

void ViewForTab::handleColorSettings()
{
    if (tabView)
        tabView->refreshVisuals();
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
	if (retVal) {
		settings.loadFromString(settingsString);
		_lastSavedSettings = settings.getVersion();
		_lastSavedModel = tabView->getVersion();
	}
	return retVal;
}

void ViewForTab::save()
{
	if(path.isNull()){
		saveAs();
		return;
	}

	if (tabView->save(path, settings.getString())) {
		_lastSavedModel = tabView->getVersion();
		_lastSavedSettings = settings.getVersion();
		logView->appendLog("Model saved", LogType::Info);
	}
	bool error;
	getModelNode(error, true); //samo da se procesuje ime modela i varijable/parametri
}

void ViewForTab::saveAs()
{
	bool error;
	getModelNode(error, true);
	path.clean();
	tabView->saveAs(settings.getString(), &path);
	_lastSavedModel = tabView->getVersion(); // u sustini mozda serviranje ne bude uspjesno trebalo bi se u prethodnoj funkciji i pointer na _lastSavedModel poslati ali u 99.99% slucajeva nije bitno, bice servirano
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

const td::String &ViewForTab::getPath()
{
    return path;
}


bool ViewForTab::promptSaveIfNecessary(bool exitProgram)
{
	if(!GlobalEvents::settingsVars.warnBeforeClose)
		return false;

	if ((_lastSavedSettings == settings.getVersion() && _lastSavedModel == tabView->getVersion()) || tabView->getVersion() == 0)
		return false;
	auto diag = new SaveDlg(this);

	diag->setResizable(false);
	diag->keepOnTopOfParent();
        
	diag->openModal([this](gui::Dialog::Button::ID id, gui::Dialog* d){
        bool close = true;
		if(id == gui::Dialog::Button::ID::OK) //save
        {
            save();
        }
		else if (id == gui::Dialog::Button::ID::Cancel) //do not save return
		{
			//saveAs();
            close = false;
			return;
		}
        
        if (close)
        {
            auto app = gui::getApplication();
            gui::Window* pMainWnd = (gui::Window*) app->getMainWnd();
            td::Variant var((void*) this);
            pMainWnd->handleUserEvent((td::UINT4) UserEvent::CloseTab, var);
//            GlobalEvents::getMainWindowPtr()->closeTab(this);
        }
        
        //if(id == gui::Dialog::Button::ID::Delete || id == gui::Dialog::Button::ID::OK) //dont save
			//GlobalEvents::getMainWindowPtr()->closeTab(this);

//        if (exitProgram && id != gui::Dialog::Button::ID::Delete) {
//			GlobalEvents::getMainWindowPtr()->prepareForClose();
//		}

	});
    return true;
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

const ModelNode& ViewForTab::getModelNode(bool &error, bool supressLogs)
{
	error = false;


	const auto& handleInvalidAttribException = [this, &error, supressLogs](ModelNode::exceptionInvalidAttribute& atr) {
		cnt::StringBuilderSmall str;
		str << "Invalid attribute \"" << atr.message << "\" on line " << atr.line;
		td::String m;
		str.getString(m);
		error = true;
		includeGuard = false;
		logView->appendLog(m, LogType::Error, supressLogs);
		return emptyModel;
	};

	const auto& handleInvalidCommandException = [this, &error, supressLogs](ModelNode::exceptionInvalidCommand& cmnd) {
		cnt::StringBuilderSmall str;
		str << "Cant process command on line " << cmnd.line << ":\n\t" << cmnd.message;
		td::String m;
		str.getString(m);
		error = true;
		includeGuard = false;
		logView->appendLog(m, LogType::Error, supressLogs);
		return emptyModel;
		};


	if (includeGuard) {
		logView->appendLog("Circular dependency detected, this model includes itself", LogType::Error, supressLogs);
		error = true;
		includeGuard = false;
		return emptyModel;
	}
	else
		includeGuard = true;

	try{
		updateModelNode();
	}catch(ModelNode::exceptionInvalidBlockName &blName){
		td::String log("Cant generate model ");
		log += name;
		log += ", unrecognized block ";
		log += blName.message;
		logView->appendLog(log, LogType::Error, supressLogs);
		error = true;
		includeGuard = false;
		return emptyModel;
	}
	catch (ModelNode::exceptionInvalidAttribute& atr) {
		handleInvalidAttribException(atr);
		return emptyModel;
	}
	catch (ModelNode::exceptionInvalidCommand& cmnd) {
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
			logView->appendLog(log, LogType::Info, supressLogs);
			model.addWtih(GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(dep.pathOrTabName), dep.alias, dep.type);
		}
		catch (MainWindow::exceptionCantFindTab &) {
			logView->appendLog("Cant find requested model, no tab with such name exists", LogType::Error, supressLogs);
			error = true;
		}
		catch (MainWindow::exceptionCantAccessFile &) {
			logView->appendLog("Cant find or access file", LogType::Error, supressLogs);
			error = true;
		}
		catch (ModelNode::exceptionInvalidBlockName& name) {
			cnt::StringBuilderSmall s;
			s << "Unrecognized block \"" << name.message << "\", Discarding model";
			td::String log;
			s.getString(log);
			logView->appendLog(log, LogType::Error, supressLogs);
			error = true;
		}
		catch (ModelNode::exceptionInvalidAttribute& atr) {
			handleInvalidAttribException(atr);
			return emptyModel;
		}
		catch (ModelNode::exceptionInvalidCommand& cmnd) {
			handleInvalidCommandException(cmnd);
			return emptyModel;
		}
		catch (...) {
			logView->appendLog("Unknown error occured, discarding model", LogType::Error, supressLogs);
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

	if(auto it = model._attribs.find("type"); it != model._attribs.end()){
		if(it->second.cCompareNoCase("NR") == 0 || it->second.cCompareNoCase("WLS") == 0)
			settings.showTimes(false);
		else
			settings.showTimes(true);
	}else
		settings.showTimes(true);

	includeGuard = false;
	return model;
}

bool ViewForTab::shouldClose()
{
    return !promptSaveIfNecessary(false);
}

ViewForTab::~ViewForTab()
{
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
