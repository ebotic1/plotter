#pragma once
#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/SplitterLayout.h>
#include <unordered_set>
#include <td/StringConverter.h>
#include <gui/StandardTabView.h>
#include <gui/Image.h>
#include <memory>
#include <gui/Application.h>
#include "GlobalEvents.h"

#include "../../EquationToXML/inc/nodes.h"
#include "ModelSettings.h"
#include "Results.h"

class ViewForTab;

class LogView : private gui::View {
	mutable gui::TextEdit textMain;
	gui::VerticalLayout _vl;

	static int errorLen, infoLen, warLen;

	friend class ViewForTab;

public:

	enum class LogType : td::BYTE { Info=0, Warning, Error };
	LogView();
	void appendLog(const td::String text, LogType type, bool discardThisLog = false) const;
	void measure(gui::CellInfo &c) override;
};

class ViewForTab : public gui::View
{
public:
	class BaseClass : public gui::View
    {
        //ViewForTab _parent = nullptr;
		unsigned int _version = 1;
		std::unordered_set<td::String> vars, params;
		void setVariabesAndParams(std::unordered_set<td::String>&& vars, std::unordered_set<td::String>&& params);
		friend class ViewForTab;
	protected:
		void modelRolledBack() {--_version; }
		void setVersion(unsigned int newVersion) {_version = newVersion;}
	public:
		void modelChanged()
        {
            //IDz: Need to send setModified flag to tabView.
            //     Very difficult with this pattern
            ++_version;
            setModified(true);
        }
        void setModified(bool bModified)
        {
            //IDz: Make class structure properly and send modified flag
            //if (_parent)
            {
                gui::Application* pApp = getApplication();
                gui::Window* pMainWnd = pApp->getInitialWindow();
                td::Variant var(bModified);
                pMainWnd->handleUserEvent((td::UINT4) UserEvent::SetModifiedFlag, var);
            }
        }
		virtual bool save(const td::String& path, const td::String& settingsString) = 0;
		virtual void saveAs(const td::String& settingsString, td::String* newPath) = 0;
		virtual void getModel(ModelNode& model) = 0;
		virtual void refreshVisuals() = 0;
		virtual bool openFile(const td::String& path, td::String& settingsString) = 0;
		unsigned int getVersion() const { return _version; }
		const std::unordered_set<td::String>& getVars();
		const std::unordered_set<td::String>& getParams();
	};
private:
	std::shared_ptr<LogView> logView;
	ModelSettings settings;
    Results _results;
	std::vector<ModelSettings::FunctionDesc> funcionsDesc;
	std::vector<ModelSettings::DependencyDesc> depenends;

	gui::SplitterLayout tabAndLogSplitter;
	gui::StandardTabView _tabView;
	gui::Image _logImg, _settingsImg, _resultsImg;
    
	td::String path, name;
	unsigned int _lastSavedModel = 0, _lastSavedSettings = 0, lastModelExtract = 0, lastSettingsVer = 0;
	ModelNode model, modelTab, emptyModel;
	bool includeGuard = false;

	BaseClass* tabView = nullptr;

	void updateModelNode();
	void updateSettings();

public:
	ViewForTab(BaseClass *, const td::String &settingsStr = td::String());

	const std::shared_ptr<LogView> getLog();
    Results* getResults();
    
	const BaseClass& getMainView();
    BaseClass* getContentView();
    void handleColorSettings();
	const td::String &getName();
	void setName(const td::String &name);
	bool loadFile(const td::String& path);
	void save();
	void saveAs();
	void exportToXML(td::String path);
	void getTimes(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations);
	void setPath(const td::String &path);
	const td::String& getPath();
	bool promptSaveIfNecessary(bool exitProgram);
	const ModelNode& getModelNode(bool &error, bool supressLogs = false);
	const std::vector<ModelSettings::FunctionDesc>& getFunctions();
	
    bool isModified() const
    {
        //IDz: ova metoda vise nije potrebna, moze se ici preko gui::TabView->isModified
        if (!tabView)
            return false;
        
        if ((_lastSavedSettings == settings.getVersion() && _lastSavedModel == tabView->getVersion()) || tabView->getVersion() == 0)
            return false;
        return true;
    }
    
	bool shouldClose() override;
	~ViewForTab();

};
