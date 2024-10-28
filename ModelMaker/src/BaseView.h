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

#include "../../EquationToXML/inc/nodes.h"
#include "ModelSettings.h"


class ViewForTab;

class LogView : private gui::View {
	mutable gui::TextEdit textMain;
	gui::VerticalLayout _vl;

	static int errorLen, infoLen, warLen;

	friend class ViewForTab;

public:

	enum class LogType { info, warning, error };
	LogView();
	void appendLog(const td::String text, LogType type, bool discardThisLog = false) const;
	void measure(gui::CellInfo &c) override;
};




class ViewForTab : public gui::View {
public:
	class BaseClass : public gui::View {
		unsigned int version = 0;
		std::unordered_set<td::String> vars, params;
		void setVariabesAndParams(std::unordered_set<td::String>&& vars, std::unordered_set<td::String>&& params);
		friend class ViewForTab;
	protected:
		void modelRolledBack() {--version; }
		void setVersion(unsigned int newVersion) {version = newVersion;}
	public:
		void modelChanged() { ++version; }
		virtual bool save(const td::String& path, const td::String& settingsString) = 0;
		virtual void saveAs(const td::String& settingsString, td::String* newPath) = 0;
		virtual void getModel(modelNode& model) = 0;
		virtual void refreshVisuals() = 0;
		virtual bool openFile(const td::String& path, td::String& settingsString) = 0;
		unsigned int getVersion() const { return version; }
		const std::unordered_set<td::String> &getVars();
		const std::unordered_set<td::String> &getParams();
	};
private:
	LogView logView;
	ModelSettings settings;
	std::vector<ModelSettings::FunctionDesc> funcionsDesc;
	std::vector<ModelSettings::DependencyDesc> depenends;

	gui::SplitterLayout tabAndLogSplitter;
	gui::StandardTabView _tabView;
	gui::Image _logImg, _settingsImg;




	td::String path, name;
	unsigned int _lastSavedModel = 0, _lastSavedSettings = 0, lastModelExtract = 0, lastSettingsVer = 0;
	modelNode model, modelTab, emptyModel;
	bool includeGuard = false;

	BaseClass* tabView = nullptr;

	void updateModelNode();
	void updateSettings();

public:
	ViewForTab(BaseClass *, const td::String &settingsStr = td::String());

	const LogView& getLog();
	const BaseClass &getMainView();

	const td::String &getName();
	void setName(const td::String &name);
	bool loadFile(const td::String& path);
	void save();
	void saveAs();
	void exportToXML(td::String path);
	void getTimes(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations);
	void setPath(const td::String &path);
	const td::String &getPath();
	bool promptSaveIfNecessary(bool exitProgram);
	const modelNode &getModelNode(bool &error, bool supressLogs = false);
	const std::vector<ModelSettings::FunctionDesc> &getFunctions();
	

	bool shouldClose() override;
	~ViewForTab();

};
