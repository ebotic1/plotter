#include "squareBlock.h"
#include "globals.h"

void squareBlock::setUpWires(bool refreshCanvas)
{
	connectionLines.clear();
	connectionLines.reserve(connectedTo.size());

	int i = 0;
	for (int i = 0; i < getOutputCnt(); i++)
	{
		const auto& par = connectedTo[i];
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
				connectionLines[i].createPolyLine(tacke, 4, 2);
			}
			else if (par.first->getOutput(par.second).x > input.x) {
				gui::Point tacke[] = { outputPoint, {outputPoint.x + i * seperatorDistance, outputPoint.y}, {outputPoint.x, input.y}, input };
				connectionLines[i].createPolyLine(tacke, 4, 2);
			}
			else {
				gui::CoordType middleY = (outputPoint.y + input.y) / 2;
				gui::Point tacke[] = { outputPoint, {outputPoint.x + i * seperatorDistance, outputPoint.y}, { outputPoint.x, middleY},  { input.x, middleY }, input };
				connectionLines[i].createPolyLine(tacke, 5, 2);
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
				connectionLines[i].createPolyLine(tacke, 4, 2);
			}
			else if (par.first->getOutput(par.second).x < input.x) {
				gui::Point tacke[] = { outputPoint, { outputPoint.x - i * seperatorDistance, outputPoint.y}, { outputPoint.x - i * seperatorDistance, input.y}, input };
				connectionLines[i].createPolyLine(tacke, 4, 2);
			}
			else {
				gui::CoordType middleY = (outputPoint.y + input.y) / 2;
				gui::Point tacke[] = { outputPoint, { outputPoint.x - i * seperatorDistance, outputPoint.y}, {  outputPoint.x - i * seperatorDistance, middleY},  { input.x, middleY }, input };
				connectionLines[i].createPolyLine(tacke, 5, 2);
			}

		}

		++i;
	}

	if (refreshCanvas)
		globals::refreshCanvas();
}

void squareBlockSI::setInputName(const td::String& name)
{
	if (name.trim().isNull())
		ulazName = "input";
	else
		ulazName = name;


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
