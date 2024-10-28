#include "ModelSettings.h"
#include <gui/GridComposer.h>
#include <regex>
#include "GlobalEvents.h"

using timeType = td::Decimal2;
auto timeTypeTD = td::DataType::decimal2;

using stepType = td::Decimal3;
auto stepTypeTD = td::DataType::decimal3;

static void setUpLineEditSize(gui::NumericEdit& edit, const td::Variant &startValue) {
	edit.setSizeLimits(55, gui::Control::Limit::UseAsMin);
	edit.setValue(startValue);
}


ModelSettings::FunctionDesc::FunctionDesc(const Type& type, const td::String& name, const td::String& yAxis, const td::String& xAxis)
	: type(type), name(name), xAxis(xAxis), yAxis(yAxis), Ycomplex(false), Xcomplex(false)
{
}

ModelSettings::ModelSettings() :
	_gridLayout(7,2),
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
	setUpLineEditSize(maxIter, td::Variant(td::UINT4(500)));

	preprocesCommands.onChangedSelection([this](){
		++version;
		auto text = preprocesCommands.getText();
		const char *start = text.begin();
		static std::cmatch match;
		static std::regex keywordPatten(R"(((?:^|\s)(import|as|function|dataset|of|versus|vs|0)|(.real|.imag))(?:$|\s))");
		static gui::Range rangeFound;
		preprocesCommands.removeColor(gui::Range(0,text.length()));
		while(std::regex_search((const td::UTF8 *)start, (const td::UTF8 *) text.end(), match, keywordPatten)){
			rangeFound.location = match[1].first - text.begin();
			rangeFound.length = match[1].length();
			preprocesCommands.setColor(rangeFound, GlobalEvents::settingsVars.colorKeyword);
			start = match.suffix().first;
	}
	
	});
	preprocesCommands.setFontSize(GlobalEvents::settingsVars.textSize);

	gui::GridComposer grid(_gridLayout);
	grid.appendRow(_lblStart) << startTime;
	grid.appendRow(_lblEnd) << endTime;
	grid.appendRow(_lblStep) << stepTime;
	grid.appendRow(_lblMaxIter) << maxIter;
	grid.startNewRowWithSpace(0,10);
	grid.appendRow(_lblPreproc);
	grid.appendEmptyCols(1);
	grid.appendRow(preprocesCommands);

	preprocesCommands.setSizeLimits(0, gui::Control::Limit::None, 100, gui::Control::Limit::Fixed);

	setLayout(&_gridLayout);
}


void ModelSettings::showTimes(bool show)
{
	startTime.show(show);
	_lblStart.show(show);
	stepTime.show(show);
	_lblStep.show(show);
	endTime.show(show);
	_lblEnd.show(show);
	
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

inline static void markComplex(td::String& name, bool& isComplex) {
	if (name.endsWith(".real"))
		name.reduceSize(5);
	else if (name.endsWith(".imag")) {
		name.reduceSize(5);
		isComplex = true;
	}
}


void ModelSettings::getFunctions(std::vector<FunctionDesc>& desc)
{
	desc.clear();
	std::regex pattern(R"((?:[^!]|)(function|dataset)\s+(?:([^;\n]+?)\s+of\s+)?([^\s\n;]+?)\s+(?:versus|vs)\s+([^\s\n;]+?)(?:\s+)?(?:[;\n]|$))");
	std::cmatch match;

	const auto& cmnds = preprocesCommands.getText();
	auto start = cmnds.begin();
	while (std::regex_search(start, cmnds.end(), match, pattern)){

		desc.emplace_back(FunctionDesc::Type::graph, match[2].matched ? td::String(match[2].first, match[2].length()) : td::String(match[3].first, match[3].length()),\
		td::String(match[3].first, match[3].length()),\
		td::String(match[4].first, match[4].length()));

		if (match[1].length() != 8) //duzina rijeci "funkcija" je 8, malo nerobusno ali dobro je
			desc.back().type = FunctionDesc::Type::points;

		start = match.suffix().first;
	}
	for (auto& d : desc) {
		if (d.name.cCompare("0") == 0)
			d.name = d.xAxis;
		markComplex(d.xAxis, d.Xcomplex);
		markComplex(d.yAxis, d.Ycomplex);
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
	maxIterations = temp.u4Val();

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
	td::String patternStr, retVal;
	std::cmatch match;

	auto getVal = [&settingsString, &pattern, &match, &patternStr](const td::String& tag, td::String &retVal){
		patternStr.format(R"(%s\(((?:\n|.)+?)\))", tag.c_str());
		pattern = patternStr.c_str();
		if (std::regex_search(settingsString.begin(), settingsString.end(), match, pattern)){
			retVal = td::String(match[1].first, match[1].length());
			return true;
		}
		return false;
	};

	if(getVal("start", retVal))
		startTime.setValue((timeType)retVal.toDouble());

	if(getVal("end", retVal))
		endTime.setValue((timeType)retVal.toDouble());

	if(getVal("step", retVal))
		stepTime.setValue((stepType)retVal.toDouble());

	if(getVal("maxIter", retVal))
		maxIter.setValue((td::UINT4)retVal.toInt());

	if(getVal("preProc", retVal))
		preprocesCommands.setText(retVal);
	

	preprocesCommands.setFontSize(GlobalEvents::settingsVars.textSize);

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
