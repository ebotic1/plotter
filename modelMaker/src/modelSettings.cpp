#include "modelSettings.h"
#include <gui/GridComposer.h>
#include <regex>


static void setUpLineEditSize(gui::NumericEdit& edit, const td::String &startValue) {
	edit.setSizeLimits(55, gui::Control::Limit::UseAsMin);
	edit.setText(startValue);
}

ModelSettings::ModelSettings() :
	_vl(6),
	_gridLayout(4,2),
	_lblStart(tr("startTime")), _lblEnd(tr("endTime")), _lblStep(tr("stepTime")),
	_lblMaxIter(tr("maxIter")), _lblPreproc(tr("preproc")),
	startTime(td::DataType::decimal2, gui::LineEdit::Messages::Send),
	endTime(td::DataType::decimal2, gui::LineEdit::Messages::Send),
	stepTime(td::DataType::decimal3, gui::LineEdit::Messages::Send),
	maxIter(td::DataType::uint4, gui::LineEdit::Messages::Send),
	preprocesCommands(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send)
{

	setUpLineEditSize(startTime, "0,00");
	setUpLineEditSize(endTime, "10,00");
	setUpLineEditSize(stepTime, "0,01");
	setUpLineEditSize(maxIter, "1");

	preprocesCommands.onChangedSelection([this](){++version;});

	gui::GridComposer grid(_gridLayout);
	grid.appendRow(_lblStart) << startTime;
	grid.appendRow(_lblEnd) << endTime;
	grid.appendRow(_lblStep) << stepTime;
	grid.appendRow(_lblMaxIter) << maxIter;

	_vl << _gridLayout;
	_vl.appendSpace(10);

	preprocesCommands.setSizeLimits(0, gui::Control::Limit::None, 100, gui::Control::Limit::Fixed);

	_vl << _lblPreproc << preprocesCommands;
	_vl.appendSpace(10);
	_vl << paramaterView;

	setLayout(&_vl);
}



void ModelSettings::getDependencies(std::vector<DependencyDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"(^import\s+(.+?)\s+(as\s+(\w))?)");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();
	int start = 0;
	while (std::regex_search(cmnds.begin() + start, cmnds.end(), match, pattern)) {
		desc.emplace_back(match[1].first, match[3].first);
		start += match[0].length();
	}

}

void ModelSettings::getFunctions(std::vector<FunctionDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"((function|dataset)\s+(\w+)\s+of\s+(\w)\s+versus\s+(\w))");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();

	while (std::regex_search(cmnds.begin(), cmnds.end(), match, pattern))
		if (match[3].matched && match[4].matched) {
			if (match[1].length() == 8) //duzina rijeci "funkcija" je 8, malo nerobusno ali dobro je
				desc.emplace_back(FunctionDesc::Type::graph, match[2].matched ? match[2].first : match[3].first, match[3].first, match[4].first);
			else
				desc.emplace_back(FunctionDesc::Type::points, match[2].matched ? match[2].first : match[3].first, match[3].first, match[4].first);
		}
}

void ModelSettings::getStartStopTime(double& startTime, double& endTime, double& stepTime, unsigned int &maxIterations)
{
	td::Variant temp;
	this->startTime.getValue(temp);
	startTime = temp.dec2Val().getAsFloat();

	this->endTime.getValue(temp);
	endTime = temp.dec2Val().getAsFloat();

	this->stepTime.getValue(temp);
	stepTime = temp.dec3Val().getAsFloat();

	this->maxIter.getValue(temp);
	//maxIterations = temp.u4Val();

}

unsigned int ModelSettings::getVersion()
{
    return version;
}

void ModelSettings::loadFromString(const td::String &settingsString)
{
	
}

const td::String ModelSettings::getString()
{
    return td::String();
}

bool ModelSettings::onChangedValue(gui::Slider *pSlider){
	++version;
    return true;
}

bool ModelSettings::onFinishEdit(gui::LineEdit *pCtrl){
	++version;
    return true;
}

bool ModelSettings::onClick(gui::CheckBox *pBtn)
{
	++version;
    return true;
}
