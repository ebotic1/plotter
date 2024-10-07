#include "modelSettings.h"
#include <gui/GridComposer.h>
#include <regex>

using timeType = td::Decimal2;
auto timeTypeTD = td::DataType::decimal2;

using stepType = td::Decimal3;
auto stepTypeTD = td::DataType::decimal3;

static void setUpLineEditSize(gui::NumericEdit& edit, const td::Variant &startValue) {
	edit.setSizeLimits(55, gui::Control::Limit::UseAsMin);
	edit.setValue(startValue);
}

ModelSettings::ModelSettings() :
	_vl(6),
	_gridLayout(4,2),
	_lblStart(tr("startTime")), _lblEnd(tr("endTime")), _lblStep(tr("stepTime")),
	_lblMaxIter(tr("maxIter")), _lblPreproc(tr("preproc")),
	startTime(timeTypeTD, gui::LineEdit::Messages::Send),
	endTime(timeTypeTD, gui::LineEdit::Messages::Send),
	stepTime(stepTypeTD, gui::LineEdit::Messages::Send),
	maxIter(td::DataType::uint4, gui::LineEdit::Messages::Send),
	preprocesCommands(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::Send)
{

	setUpLineEditSize(startTime,  td::Variant(timeType(0.0)));
	setUpLineEditSize(endTime,  td::Variant(timeType(10.0)));
	setUpLineEditSize(stepTime,  td::Variant(stepType(0.01)));
	setUpLineEditSize(maxIter, td::Variant(td::UINT4(1)));

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
	std::regex pattern(R"(import\s+([^;\n]+?)(?:\s+as\s+(\w+))?(?:\s+)?(?:[;\n]|$))");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();
	auto start = cmnds.begin();
	while (std::regex_search(start, cmnds.end(), match, pattern)) {
		desc.emplace_back(match[1].first, match[1].length(),  match[2].first, match[2].length());
		start = match.suffix().first;
	}

}

void ModelSettings::getFunctions(std::vector<FunctionDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"((?:[^!]|)(function|dataset)\s+(?:([^;\n]+?)\s+of\s+)?([^\s\n;]+?)\s+versus\s+([^\s\n;]+?)(?:\s+)?(?:[;\n]|$))");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();
	auto start = cmnds.begin();
	while (std::regex_search(start, cmnds.end(), match, pattern)){
		if (match[1].length() == 8) //duzina rijeci "funkcija" je 8, malo nerobusno ali dobro je
			desc.emplace_back(FunctionDesc::Type::graph, match[2].matched ? match[2].first : match[3].first, match[3].first, match[4].first);
		else
			desc.emplace_back(FunctionDesc::Type::points, match[2].matched ? match[2].first : match[3].first, match[3].first, match[4].first);
		start = match.suffix().first;
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
	if(settingsString.isNull())
		return;
	
	std::regex pattern;
	std::cmatch match;

	pattern = R"(start(.+?))";
	if (std::regex_search(settingsString.begin(), settingsString.end(), match, pattern))
		startTime.setValue(timeType(td::String(match[1].first, match[1].length()).toDouble()), false);

	

	++version;

}

const td::String ModelSettings::getString()
{
    cnt::StringBuilder s; //format: start(0)end(10)step(1)...
	s << "start(" << startTime.getValue()\
	<< ")end(" << endTime.getValue()\
	<< ")step(" << stepTime.getValue()\
	<< ")maxIter(" << maxIter.getValue()\
	<< ")preProc(" << preprocesCommands.getText()\
	<< ")";
	td::String str;
	s.getString(str);
	return str;
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


ModelSettings::DependencyDesc::DependencyDesc(const char *path, int str1Size, const char *alias, int str2Size):
	pathOrTabName(path, str1Size),
	alias(alias, str2Size)
{
}
