#include "squareBlock.h"
#include "globals.h"

inline void setUpTextRect(gui::DrawableString &string, gui::Rect &areaRect, const bool &switchOutput, const gui::Point &origin, const gui::Rect _r) {
	gui::Size sz;
	constexpr int offset = 15;

	string.measure(FONT_ID, sz);

	const double width = (switchOutput ? -origin.x : origin.x) - (switchOutput ? -_r.left : _r.right);

	areaRect.setOrigin({ origin.x - width / 2,  origin.y - offset - sz.height });
	areaRect.setWidth(width);
}


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

			constexpr int seperatorDistance = 28;

			int direction = (par.first->getInputSwitched()) ? -1 : 1;
			int logicalIndex = (outputPoint.y > input.y) ? (par.first->getInputCnt() - par.second - 1) : par.second;
			logicalIndex *= direction;
			

			if (outputPoint.x < input.x) { // linija ide prema desno
				if (outputPoint.x > inputPoint.x) {
					gui::Point tacke[] = { outputPoint,  { input.x - logicalIndex * seperatorDistance, outputPoint.y }, {input.x - logicalIndex * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else if (direction==1){
					gui::Point tacke[] = { outputPoint, {outputPoint.x , input.y}, {outputPoint.x, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else {
					
					gui::CoordType middleY = (outputPoint.y > input.y) ? (_r.top - 0.2 * (-par.first->getInput(0).y + _r.top)) : (_r.bottom - 0.2 * (-par.first->getInput(par.first->getInputCnt() - 1).y + _r.bottom));
					gui::Point tacke[] = { outputPoint, { outputPoint.x, middleY},  { input.x - logicalIndex * seperatorDistance, middleY },  { input.x - logicalIndex * seperatorDistance, input.y }, input };
					connectionLines[j].createPolyLine(tacke, 5, 2);
				}

			}
			else { //linija ide prema lijevo

				if (outputPoint.x < inputPoint.x) {
					gui::Point tacke[] = { outputPoint,  { input.x + par.second * seperatorDistance, outputPoint.y},  {input.x + par.second * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else if (par.first->getInputSwitched()) {
					gui::Point tacke[] = { outputPoint, { outputPoint.x - i * seperatorDistance, outputPoint.y}, { outputPoint.x - i * seperatorDistance, input.y}, input };
					connectionLines[j].createPolyLine(tacke, 4, 2);
				}
				else {
					gui::CoordType middleY = (outputPoint.y + par.first->getInput(par.first->getInputCnt() - par.second - 1).y ) / 2;
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
	_r.setHeight(100 + (cnt-1) * 50);
	double distanceInput = _r.height() / (getInputCnt() + 1);
	double distanceOutput = _r.height() / (getOutputCnt() + 1);



	const gui::CoordType armLenght = _r.height() * 0.1 + 60;

	gui::Point inputPoint, outputPoint;

	inputPoint.x = switchOutput ? _r.right : _r.left;
	inputPoint.y = _r.top + distanceInput;

	outputPoint.x = switchOutput ? _r.left : _r.right;
	outputPoint.y = _r.top + distanceOutput;


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

void squareBlockSI::drawBlock(td::ColorID color)
{
	if (drawUlaz.isInitialized() && !getIsConnectedFrom())
		drawUlaz.draw(inputRect, &blockFont, color, td::TextAlignment::Center);
}

void squareBlockSI::setUpBlock()
{
	drawUlaz = ulazName;
	setUpTextRect(drawUlaz, inputRect, !switchOutput, getInput(0), _r);
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

void squareBlockSO::drawBlock(td::ColorID color)
{

	if (drawIzlaz.isInitialized() && !getIsConnectedTo())
		drawIzlaz.draw(outputRect, &blockFont, color, td::TextAlignment::Center);

}

void squareBlockSO::setUpBlock()
{
	drawIzlaz = izlazName;
	setUpTextRect(drawIzlaz, outputRect, switchOutput, getOutput(0), _r);
}










squareBlockMInameless::squareBlockMInameless(int input_cnt): 
	inputCnt(input_cnt)
{
	connectedFrom.resize(inputCnt);
}

const td::String& squareBlockMInameless::getInputName(int pos) const
{
	static_assert("Cant access nameless input name");
	return "";
}

void squareBlockMInameless::changeInputCnt(int cnt)
{
	if (inputCnt > cnt)
		for (int i = cnt; i < inputCnt; ++i)
			disconnectInput(i);
	
	inputCnt = cnt;
	connectedFrom.resize(cnt);
	setUpAll();
}

void squareBlockMInameless::updateSettingsView(BlockBase::settingsView* view)
{
	auto pogled = dynamic_cast<settingsView*>(view);
	pogled->currentBlockc = this;
	pogled->cntEdit.setValue(inputCnt, false);
}







squareBlockMI::squareBlockMI(int inputs_cnt): inputCnt(inputs_cnt), names(inputs_cnt)
{
}

const td::String& squareBlockMI::getInputName(int pos) const
{
	return names.at(pos);
}

void squareBlockMI::changeInputCnt(int cnt)
{
	names.resize(cnt);
}

void squareBlockMI::updateSettingsView(BlockBase::settingsView* view)
{
	auto pogled = (settingsView*)view;

	delete(pogled->dynamicVL);
	pogled->dynamicVL = new gui::VerticalLayout(inputCnt);
	pogled->inputs.clear();

	td::String label;
	for (int i = 0; i < names.size(); ++i) {
		label.format("input name %d:", i);
		pogled->inputs.emplace_back(label, td::string8, "variable name of this block's input", names[i]);
		pogled->inputs.back().Action = [this, i](const td::Variant& v) {setInputName(v.strVal(), i); };

		pogled->dynamicVL->append(pogled->inputs.back());
	}

}


void squareBlockMI::setInputName(const td::String& name, int pos)
{
	names.at(pos) = name;
}

