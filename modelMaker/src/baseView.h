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

	friend class ViewForTab;

public:

	enum class LogType { info, warning, error };
	LogView();
	void appendLog(const td::String text, LogType type) const;
};




class ViewForTab : public gui::View {
public:
	class BaseClass : public gui::View {
		long long unsigned int version = 1;
	public:
		virtual bool save(const td::String& path) { return false; }
		virtual td::String saveAs() { return ""; }
		virtual void getModel(modelNode& model) {}
		virtual bool openFile(const td::String& path) { return false; }
		bool modelChanged() { ++version; }
		long long unsigned int getVersion() const { return version; }
	};
private:
	LogView logView;
	ModelSettings settings;

	gui::SplitterLayout mainView;
	gui::HorizontalLayout _hLayout;

	td::String path;
	long long unsigned int lastSaved = 0, lastModelExtract = 0;
	modelNode model;

	BaseClass* tabView = nullptr;

	void updateModelNode();

public:

	ViewForTab(BaseClass *);

	const LogView& getLog() {
		return logView;
	}

	bool loadFile(const td::String& path);
	bool save();
	bool saveAs();
	void exportToXML(td::String path);
	void getTimes(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations);
	const modelNode &getModelNode();
	long long unsigned int getVersion();

	~ViewForTab();

};