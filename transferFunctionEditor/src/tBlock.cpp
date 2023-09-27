#include "tBlock.h"
#include "globals.h"

const gui::Rect& Block::getRect() const{
	return _r;
}

const bool Block::getInputSwitched() const{
	return switchOutput;
}


const gui::Point &Block::getOutput() const{
	return outputPoint;
}

const gui::Point &Block::getInput() const{
	return inputPoint;
}

const gui::Point& Block::getLocation() const{
	return gui::Point(_r.left, _r.top);
}

const std::vector<Block*> &Block::getConnectedBlocks() const{
	return connectedTo;
}

const std::vector<Block*>& Block::getConnectedFromBlocks() const{
	return connectedFrom;
}

const td::String& Block::getOutputName() const{
	return izlazName;
}

const td::String& Block::getInputName() const{
	return ulazName;
}


bool Block::intersectsInput(const gui::Point& p) {
	return gui::Circle(inputPoint, 20).contains(p);
}

bool Block::intersectsOutput(const gui::Point& p) {
	return gui::Circle(outputPoint, 14).contains(p);
}

bool Block::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}

void Block::getAllProps(td::String& nominator, td::String& denominator, bool& connected, bool& switchedInput, td::String& inputName, td::String& outputName){
	nominator = nom;
	denominator = dem;
	switchedInput = this->switchOutput;
	connected = (connectedFrom.size() > 0 || connectedTo.size() > 0) ? true : false;
	inputName = ulazName;
	outputName = izlazName;
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

void Block::setInputName(const td::String& name){
	if (name.trim().isNull())
		ulazName = "input";
	else
		ulazName = name;

	drawUlaz = ulazName;
	globals::refreshCanvas();
}

void Block::setOutputName(const td::String& name){
	if (name.trim().isNull())
		izlazName = "output";
	else
		izlazName = name;

	drawIzlaz = izlazName;

	globals::refreshCanvas();
}

void Block::connectTo(Block* block) {
	if (block == this)
		return;

	auto iterator = std::find(connectedTo.begin(), connectedTo.end(), block);
	if (connectedTo.end() != iterator)
		return;
	connectedTo.push_back(block);
	block->connectedFrom.push_back(this);

	setUpWires(true);
}

void Block::removeConnections() {
	for(Block *var : connectedTo){
		var->connectedFrom.erase(std::find(var->connectedFrom.begin(), var->connectedFrom.end(), this));

	}
	connectedTo.clear();
	for(Block * var : connectedFrom) {
		var->connectedTo.erase(std::find(var->connectedTo.begin(), var->connectedTo.end(), this));
		var->setUpWires(false);
	}
	connectedFrom.clear();
	setUpWires(true);
}


void Block::switchInput() {
	switchOutput = !switchOutput;
	setUpAll();
}

void Block::setPosition(const gui::Point& position){
	_r.setOrigin(position);
	setUpAll();
}


void Block::setUpAll() {

	if (disableSetUp)
		return;

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

	{
		gui::Size sz;
		constexpr int offset = 13;
		drawUlaz.measure(FONT_ID, sz);

		inputRect.setOrigin({ inputPoint.x - armLenght / 1.1, inputPoint.y - sz.height - offset});
		inputRect.setWidth(armLenght*2/1.2);

		drawIzlaz.measure(FONT_ID, sz);

		outputRect.setOrigin({ outputPoint.x - armLenght / 1.1, outputPoint.y - sz.height - offset});
		outputRect.setWidth(armLenght*2/1.2);
	}

	for(Block * var : connectedFrom)
		var->setUpWires(false);

	setUpWires(true); // will refresh canvas

}

void Block::setUpWires(bool refreshCanvas){

	if (disableSetUp)
		return;

	connectionLines.resize(connectedTo.size(), gui::Shape());
	for (int i = 0; i < connectionLines.size(); ++i) {
		gui::Point& input = connectedTo[i]->inputPoint;
		connectionLines[i] = gui::Shape();

		if (outputPoint.x < input.x) { // linija ide prema desno
			if (outputPoint.x > inputPoint.x) {
				gui::Point tacke[] = { outputPoint,  { input.x, outputPoint.y }, input };
				connectionLines[i].createPolyLine(tacke, 3, 2);
			}
			else if(connectedTo[i]->outputPoint.x > input.x){
				gui::Point tacke[] = { outputPoint,  { outputPoint.x, input.y }, input };
				connectionLines[i].createPolyLine(tacke, 3, 2);
			}
			else {
				gui::CoordType middleY = (outputPoint.y + input.y) / 2; 
				gui::Point tacke[] = { outputPoint,  { outputPoint.x, middleY},  { input.x, middleY }, input };
				connectionLines[i].createPolyLine(tacke, 4, 2);
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
				gui::Point tacke[] = { outputPoint,  { input.x, outputPoint.y}, input };
				connectionLines[i].createPolyLine(tacke, 3, 2);
			}
			else if (connectedTo[i]->outputPoint.x < input.x) {
				gui::Point tacke[] = { outputPoint,  { outputPoint.x, input.y}, input };
				connectionLines[i].createPolyLine(tacke, 3, 2);
			}else{
				gui::CoordType middleY = (outputPoint.y + input.y) / 2;
				gui::Point tacke[] = { outputPoint,  { outputPoint.x, middleY},  { input.x, middleY }, input };
				connectionLines[i].createPolyLine(tacke, 4, 2);
			}

		}

	}

	if(refreshCanvas)
		globals::refreshCanvas();
}


void Block::drawBlock(td::ColorID color) {
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
	else {
		drawUlaz.draw(inputRect, FONT_ID, color, td::TextAlignment::Center);
	}
	if(connectedTo.size() == 0)
		drawIzlaz.draw(outputRect, FONT_ID, color, td::TextAlignment::Center);
	

}

void Block::disableLogic(bool disable){
	disableSetUp = disable;
}


Block::Block(const gui::Point& position, const td::String& inputName, const td::String& outputName){
	_r.setOrigin(position);
	setNominator("1");
	setDenominator("s");

	setInputName(inputName);
	setOutputName(outputName);

	setUpAll();
}


Block::~Block(){
	removeConnections();
}