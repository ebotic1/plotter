#pragma once
#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/GridLayout.h>
#include <vector>
#include <gui/Label.h>
#include <gui/NumericEdit.h>
#include "../../EquationToXML/inc/nodes.h"

class ModelSettings;

class SyntaxText: public gui::TextEdit{
	ModelSettings *_parent;
	void highlightText();
public:
	SyntaxText(ModelSettings *parent);
	bool onKeyPressed(const gui::Key &k) override;
	void setText(const td::String &text);

};

class ModelSettings : public gui::View {
	gui::GridLayout _gridLayout;
	gui::Label _lblStart, _lblEnd, _lblStep, _lblMaxIter, _lblPreproc;
	SyntaxText preprocesCommands;
	gui::NumericEdit startTime, endTime, stepTime, maxIter;

	gui::View paramaterView;
	unsigned int version = 1;
	friend class SyntaxText;

public:
	struct FunctionDesc {
		enum class Type{graph, points} type;
		td::String name, yAxis, xAxis;
		bool Ycomplex = false, Xcomplex = false;
		FunctionDesc(const Type& type, const td::String& name, const td::String& yAxis, const td::String& xAxis);
	};
	struct DependencyDesc {
		td::String pathOrTabName, alias;
		modelNode::addType type;
		DependencyDesc(const char* path, int str1Size, const char* alias, int str2Size, modelNode::addType);
	};

public:
	ModelSettings();
	void showTimes(bool show);
	void getDependencies(std::vector<DependencyDesc> &);
	void getFunctions(std::vector<FunctionDesc>&);
	void getStartStopTime(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations);
	unsigned int getVersion();

	void loadFromString(const td::String &settingsString);
	const td::String getString();

	virtual bool onChangedValue(gui::Slider* pSlider) override;
	virtual bool onFinishEdit(gui::LineEdit* pCtrl) override;
	virtual bool onClick(gui::CheckBox* pBtn) override;

};