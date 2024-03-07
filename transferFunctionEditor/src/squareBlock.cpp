#include "squareBlock.h"
#include "globals.h"


squareBlock::squareBlock()
{
}

void squareBlock::setUpWires(bool refreshCanvas)
{
	connectionLines.clear();

	int cnt = 0;
	for (const auto& connection : connectedTo)
		cnt += connection.size();

	connectionLines.reserve(cnt);

	int j = 0;
	for (int i = 0; i < getOutputCnt(); i++)
	{
		
		for (const auto& par : connectedTo[i])
		{
			if (par.first == nullptr)
				continue;
			connectionLines.resize(connectionLines.size() + 1);
			const auto& outputPoint = getOutput(i);
			const gui::Point& inputPoint = (getInputCnt() > 0) ? getInput(0) : _r.center();
			const gui::Point& input = par.first->getInput(par.second);

			constexpr int seperatorDistance = 5;

			if (outputPoint.x < input.x) { // linija ide prema desno
				if (outputPoint.x > inputPoint.x) {
					gui::Point tacke[] = { outputPoint,  { input.x - par.second * seperatorDistance, outputPoint.y }, {input.x - par.second * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else if (par.first->getOutput(par.second).x > input.x) {
					gui::Point tacke[] = { outputPoint, {outputPoint.x + i * seperatorDistance, outputPoint.y}, {outputPoint.x, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else {
					gui::CoordType middleY = (outputPoint.y + input.y) / 2;
					gui::Point tacke[] = { outputPoint, {outputPoint.x + i * seperatorDistance, outputPoint.y}, { outputPoint.x, middleY},  { input.x, middleY }, input };
					connectionLines[j].createPolyLine(tacke, 5, 2);
				}


				/*
				*
				* moze se i ovo koristiti. Kada se spaja blok prelama liniju na pola puta umjesto na kraju
				*
				gui::CoordType middle = (outputPoint.x + input.x) / 2;  //(input.x - outputPoint.x)/2 + outputPoint.x ;
				gui::Point tacke[] = { outputPoint,  { middle, outputPoint.y },  { middle, input.y }, input };
				connectionLines[i].createPolyLine(tacke, 4, 2);
				*/
			}
			else { //linija ide prema lijevo

				if (outputPoint.x < inputPoint.x) {
					gui::Point tacke[] = { outputPoint,  { input.x + par.second * seperatorDistance, outputPoint.y},  {input.x + par.second * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else if (par.first->getOutput(par.second).x < input.x) {
					gui::Point tacke[] = { outputPoint, { outputPoint.x - i * seperatorDistance, outputPoint.y}, { outputPoint.x - i * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else {
					gui::CoordType middleY = (outputPoint.y + input.y) / 2;
					gui::Point tacke[] = { outputPoint, { outputPoint.x - i * seperatorDistance, outputPoint.y}, {  outputPoint.x - i * seperatorDistance, middleY},  { input.x, middleY }, input };
					connectionLines[j].createPolyLine(tacke, 5, 2);
				}

			}
			++j;
		}
	}

	if (refreshCanvas)
		globals::refreshCanvas();
}


void squareBlock::createArrow(gui::Shape& arrow, const gui::Point& posBegin, const gui::Point& posEnd, const double &lenght)
{
	const gui::CoordType arrowLenght = lenght * 0.1875;
	const char direction = (posBegin.x < posEnd.x) ? 1 : -1;

	gui::Point tacke[] = { posBegin,  posEnd, {posEnd.x - arrowLenght * direction, posEnd.y - arrowLenght}, posEnd, {posEnd.x - arrowLenght * direction, posEnd.y + arrowLenght} };
	arrow.createPolyLine(tacke, 5, 2);
}


void squareBlock::setUpBlock()
{
	const int& cnt = (getInputCnt() > getOutputCnt()) ? getInputCnt() : getOutputCnt();
	_r.setHeight(cnt * 100);
	const double distanceInput = _r.height() / (getInputCnt() + 1);
	const double distanceOutput = _r.height() / (getOutputCnt() + 1);


	const gui::CoordType armLenght = _r.height() / 1.5;

	gui::Point inputPoint, outputPoint;

	inputPoint.x = _r.left;
	inputPoint.y = _r.top + distanceInput;

	outputPoint.x = _r.right;
	outputPoint.y = _r.top + distanceInput;

	if (switchOutput)
		std::swap(inputPoint, outputPoint);
	const char direction = (switchOutput) ? -1 : 1;

	arrows.clear();
	arrows.resize(getInputCnt() + getOutputCnt());

	inputPoints.resize(getInputCnt());
	outputPoints.resize(getOutputCnt());

	int i = 0;
	for (; i < getInputCnt(); ++i) {
		inputPoints[i] = { inputPoint.x - armLenght * direction , inputPoint.y + i * distanceInput };
		createArrow(arrows[i], inputPoints[i], { inputPoint.x, inputPoint.y + i * distanceInput }, armLenght);
	}

	for (int j = 0; j < getOutputCnt(); ++j) {
		outputPoints[j] = { outputPoint.x + armLenght * direction , outputPoint.y + j * distanceOutput };
		createArrow(arrows[i + j], { outputPoint.x, outputPoint.y + j * distanceInput }, outputPoints[j], armLenght);
	}

	recShape.createRect(_r);
}




void squareBlock::drawBlock(td::ColorID color)
{
	recShape.drawWire(color);
	for (const auto& arrow : arrows)
		arrow.drawWire(color);

}


void squareBlockSI::setInputName(const td::String& name)
{
	if (name.trim().isNull())
		ulazName = "input";
	else
		ulazName = name;

	if (!disableSetUp)
		setUpAll();
}

void squareBlockSI::updateSettingsView(BlockBase::settingsView* view)
{
	auto viewSI = dynamic_cast<settingsView*>(view);
	if (viewSI == nullptr) return;

	viewSI->inputProp.Action = [this](const td::Variant& v) {this->setInputName(v.strVal()); };
	viewSI->inputProp.setValue(ulazName);
}



squareBlockSO::squareBlockSO(const td::String& outputName):
	squareBlockSO()
{
	izlazName = outputName;
}

void squareBlockSO::setOutputName(const td::String& name)
{
	if (name.trim().isNull())
		izlazName = "output";
	else
		izlazName = name;

	if (!disableSetUp)
		setUpAll();
}

void squareBlockSO::updateSettingsView(BlockBase::settingsView* view)
{
	auto viewSO = dynamic_cast<settingsView*>(view);
	if (viewSO == nullptr) return;

	viewSO->outputProp.Action = [this](const td::Variant& v) {this->setOutputName(v.strVal()); };
	viewSO->outputProp.setValue(izlazName);
}


