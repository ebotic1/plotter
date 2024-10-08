#pragma once
#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/SplitterLayout.h>

#include "../../EquationToXML/inc/nodes.h"
#include "modelSettings.h"


class ViewForTab;

class LogView : private gui::View {
	mutable gui::TextEdit textMain;
	gui::VerticalLayout _vl;

	static int errorLen, infoLen, warLen;

	friend class ViewForTab;

public:

	enum class LogType { info, warning, error };
	LogView();
	void appendLog(const td::String text, LogType type) const;
};




class ViewForTab : public gui::View {
public:
	class BaseClass : public gui::View {
		unsigned int version = 1;
	protected:
		void modelChanged() { ++version; }
		void modelRolledBack() {--version; }
	public:
		virtual bool save(const td::String& path, const td::String& settingsString) { return false; }
		virtual void saveAs(const td::String& settingsString, td::String *newPath) {}
		virtual void getModel(modelNode& model) {}
		virtual bool openFile(const td::String& path, td::String& settingsString) { return false; }
		unsigned int getVersion() const { return version; }
	};
private:
	LogView logView;
	ModelSettings settings;
	std::vector<ModelSettings::FunctionDesc> funcionsDesc;
	std::vector<ModelSettings::DependencyDesc> depenends;

	gui::SplitterLayout mainView, tabAndLogSplitter;
	gui::View tabAndLogView;



	td::String path, name;
	unsigned int lastSaved = 0, lastModelExtract = 0, lastSettingsVer = 0;
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
	const modelNode &getModelNode(bool &error);

	~ViewForTab();

};