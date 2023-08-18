#include "tBlock.h"
#include "globals.h"

const gui::Rect& Block::getRect() const{
	return _r;
}

const bool Block::getInputSwitched() const{
	return switchOutput;
}

const gui::Point Block::getMostRightPoint() const{
	return switchOutput ? inputPoint : outputPoint;
}

const gui::Point &Block::getOutput() const{
	return outputPoint;
}

const gui::Point &Block::getInput() const{
	return inputPoint;
}


bool Block::intersectsInput(const gui::Point& p) {
	return gui::Circle(inputPoint, 20).contains(p);
}

bool Block::intersectsOutput(const gui::Point& p) {
	return gui::Circle(outputPoint, 14).contains(p);
}

void Block::getAllProps(td::String& nominator, td::String &denominator, bool& connected, bool &switchedInput){
	nominator = nom;
	denominator = dem;
	switchedInput = this->switchOutput;
	connected = (connectedFrom.size() > 0 || connectedTo.size() > 0) ? true : false;
}

void Block::setNominator(const td::String& nominator){
	nom = nominator.isNull() ? "1" : nominator;
	drawNom = nom;
	setUpAll();
}

void Block::setDenominator(const td::String& denominator){
	dem = denominator.isNull() ? "1" : denominator;
	drawDem = dem;
	setUpAll();
}

void Block::connectTo(Block* block){
	if (block == this)
		return;
	auto iterator = std::find(block->connectedTo.begin(), block->connectedTo.end(), this);
	if (block->connectedTo.end() != iterator)
		return;
	iterator = std::find(connectedTo.begin(), connectedTo.end(), block);
	if (connectedTo.end() != iterator)
		return;
	connectedTo.push_back(block);
	block->connectedFrom.push_back(this);

	setUpWires();
}

void Block::drawBlock(td::ColorID color){
	recShape.drawWire(color);
	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		drawNom.draw(rectangleNominator, FONT_ID, color, td::TextAlignment::Center);
		drawDem.draw(rectangleDenominator, FONT_ID, color, td::TextAlignment::Center);
		fracLine.drawWire(color, 2);
	}

	for (int i = 0; i < connectionLines.size(); ++i)
		connectionLines[i].drawWire(color);

	if (connectedFrom.size() != 0) {
		gui::Shape dot;
		dot.createCircle(gui::Circle(inputPoint, 4), 1);
		dot.drawFill(color);
	}

}

bool Block::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}



void Block::switchInput() {
	switchOutput = !switchOutput;
	setUpAll();
}

void Block::setPosition(const gui::Point& position){
	_r.setOrigin(position);
	setUpAll();
	for each (Block * var in connectedFrom)
		var->setUpWires();
}

void Block::setUpAll() {

	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		gui::Size s1, s2;
		drawNom.measure(FONT_ID, s1);
		drawDem.measure(FONT_ID, s2);

		gui::CoordType& bigWidth = s1.width > s2.width ? s1.width : s2.width;

		_r.setWidth( 100 + bigWidth);
		_r.setHeight(2 * s1.height + 50);

		constexpr int offset = 6;

		rectangleNominator.setOrigin({ _r.left, _r.center().y - offset - s1.height});
		rectangleNominator.setSize({_r.width(), 0});

		rectangleDenominator.setOrigin({ _r.left, _r.center().y + offset});
		rectangleDenominator.setSize({ _r.width(), 0});

		gui::Point p[] = { {_r.center().x - bigWidth/2 - offset, _r.center().y}, {_r.center().x + bigWidth/2 + offset, _r.center().y}};
		fracLine.createLines(p, 2, 1);

	}
	else{
		_r.setWidth(100);
		_r.setHeight(50);
	}


	gui::Point centar = _r.center();
	const gui::CoordType arrowLenght = _r.height() / 8;
	const gui::CoordType armLenght = _r.height() / 1.5;

	inputPoint.x = _r.left - armLenght;
	inputPoint.y = centar.y;

	outputPoint.x = _r.right + armLenght;
	outputPoint.y = centar.y;

	if (switchOutput)
		std::swap(inputPoint, outputPoint);


	if (switchOutput) {
		gui::Point tacke[] = { {_r.left, _r.bottom},  {_r.left, centar.y}, outputPoint,  {_r.left - armLenght + arrowLenght, centar.y + arrowLenght},
		outputPoint,  {_r.left - armLenght + arrowLenght, centar.y - arrowLenght}, outputPoint, {_r.left, centar.y}, {_r.left, _r.top},
		{_r.right, _r.top}, {_r.right, centar.y},{_r.right + arrowLenght, centar.y + arrowLenght}, {_r.right, centar.y}, {_r.right + arrowLenght, centar.y - arrowLenght},
		{_r.right, centar.y}, inputPoint, {_r.right, centar.y}, {_r.right, _r.bottom}, {_r.left, _r.bottom} };
		recShape.createPolyLine(tacke, 19, 2);

	}
	else {
		gui::Point tacke[] = { {_r.left, _r.bottom},  {_r.left, centar.y}, {_r.left - arrowLenght,centar.y + arrowLenght}, {_r.left, centar.y},   {_r.left - arrowLenght,centar.y - arrowLenght},
			{_r.left, centar.y}, inputPoint, {_r.left, centar.y}, {_r.left, _r.top}, {_r.right, _r.top}, {_r.right, centar.y},
			outputPoint, {_r.right + armLenght - arrowLenght, centar.y + arrowLenght}, outputPoint,
			{_r.right + armLenght - arrowLenght, centar.y - arrowLenght}, outputPoint, {_r.right, centar.y}, {_r.right, _r.bottom}, {_r.left, _r.bottom} };
		recShape.createPolyLine(tacke, 19, 2);
	}


	setUpWires(); // will refresh canvas

}

void Block::setUpWires(){

	connectionLines.resize(connectedTo.size(), gui::Shape());
	for (int i = 0; i < connectionLines.size(); ++i) {
		gui::Point& input = connectedTo[i]->inputPoint;
		connectionLines[i] = gui::Shape();

		if (outputPoint.x < input.x) {
			gui::CoordType middle = (outputPoint.x + input.x) / 2;  //(input.x - outputPoint.x)/2 + outputPoint.x ;
			gui::Point tacke[] = { outputPoint,  { middle, outputPoint.y },  { middle, input.y }, input };
			connectionLines[i].createPolyLine(tacke, 4, 2);
		}
		else {
			gui::CoordType middleY = (outputPoint.y + input.y) / 2;
			gui::Point tacke[] = { outputPoint,  { outputPoint.x, middleY},  { input.x, middleY }, input };
			connectionLines[i].createPolyLine(tacke, 4, 2);
		}

	}

	globals::refreshCanvas();
}


Block::Block(const gui::Point& position){
	_r.setOrigin(position);
	setNominator("1");
	setDenominator("s");
	setUpAll();
}