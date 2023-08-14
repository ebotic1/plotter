#pragma once
#include "gui/Shape.h"

class block {
	gui::Rect _r;
	gui::Shape recShape;
	bool switchOutput = false;
	gui::Point inputPoint, outputPoint;

public:
	block(gui::CoordType x = 0, gui::CoordType y = 0, gui::CoordType width = 0, gui::CoordType height = 0);
	const gui::Shape &getShape() const{
		return recShape;
	}

	void setUpShape();
	void switchInput();

	bool intersectsBlock(const gui::Point &);
	bool intersectsInput(const gui::Point &);
	bool intersectsOutput(const gui::Point&);

};

inline bool block::intersectsInput(const gui::Point &p){
	return gui::Circle(inputPoint, 10).contains(p);
}

inline bool block::intersectsOutput(const gui::Point &p){
	return gui::Circle(outputPoint, 10).contains(p);
}

inline bool block::intersectsBlock(const gui::Point &p){
	return _r.contains(p);
}

inline void block::switchInput(){
	switchOutput = !switchOutput;
	std::swap(inputPoint, outputPoint);
}

void block::setUpShape(){
	gui::Point centar = _r.center();
	const gui::CoordType arrowLenght = _r.height() / 14;
	const gui::CoordType armLenght = _r.height() / 5;

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
	
}

block::block(gui::CoordType x, gui::CoordType y, gui::CoordType width, gui::CoordType height) {
	_r.setLeftAndWidth(x, width);
	_r.setTopAndHeight(y, height);
	setUpShape();
}