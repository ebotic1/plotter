#include "tBlock.h"

const gui::Shape& Block::getShape() const{
	return recShape;
}

bool Block::intersectsInput(const gui::Point& p) {
	return gui::Circle(inputPoint, 10).contains(p);
}

bool Block::intersectsOutput(const gui::Point& p) {
	return gui::Circle(outputPoint, 10).contains(p);
}

bool Block::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}

inline void Block::switchInput() {
	switchOutput = !switchOutput;
	std::swap(inputPoint, outputPoint);
}

void Block::setUpShape() {
	gui::Point centar = _r.center();
	const gui::CoordType arrowLenght = _r.height() / 12;
	const gui::CoordType armLenght = _r.height() / 3;

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

Block::Block(gui::CoordType x, gui::CoordType y, gui::CoordType width, gui::CoordType height) {
	_r.setLeftAndWidth(x, width);
	_r.setTopAndHeight(y, height);
	setUpShape();
}