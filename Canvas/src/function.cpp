#include "./../inc/function.h"


gui::Point Function::findIntersection(const gui::Point& p1, const gui::Point& p2, const gui::Rect &r) // q1 i q2 su ravni rect sa tackama koji prikazuju dole, desno
{

	auto linesIntersect = [](const gui::Point& p1, const gui::Point& p2, const gui::Point& q1, const gui::Point& q2) -> gui::Point{

		bool verticalSide = (q1.y == q2.y) ? false : true;

		if (p1.y == p2.y) {
			if (!verticalSide)
				if (p1.y == q1.y)
					if (p1.x < p2.x)
						return { q2.x, p1.y };
					else
						return { q1.x, p1.y };
				else
					return { q1.x-1, q1.y-1 };

			return { q1.x, p1.y };

		}

		if (p1.x == p2.x) {
			if (verticalSide)
				if (p1.x == q1.x)
					if (p1.y < p2.y)
						return { p1.x, q2.y };
					else
						return { p1.x, q1.y };
				else
					return { q1.x - 1, q1.y - 1 };

			return { p1.x, q1.y };
		}

		const gui::CoordType m = (p1.y - p2.y) / (p1.x - p2.x);
		const gui::CoordType c = p1.y - p1.x * m;
		if (verticalSide)
			return { q1.x, m * q1.x + c };
		return { (q1.y - c) / m, q1.y };
		};


	gui::Point p;
	
	const gui::Point& outsidePoint = r.contains(p1) ? p2 : p1;
	
	if (outsidePoint.y <= r.bottom && outsidePoint.x >= r.left) {
		p = linesIntersect(p1, p2, { r.left, r.top }, { r.right, r.top });
		if (p.x >= r.left && p.x <= r.right)
			return p;
		return linesIntersect(p1, p2, { r.right, r.top }, { r.right, r.bottom });
	}
	else {
		p = linesIntersect(p1, p2, { r.left, r.bottom }, { r.right, r.bottom });
		if (p.x >= r.left && p.x <= r.right)
			return p;
		return linesIntersect(p1, p2, { r.left, r.top }, { r.left, r.bottom });
	}


}

Function::Function(gui::CoordType* x, gui::CoordType* y, size_t length, td::ColorID color, const td::String& name, double lineWidth, td::LinePattern pattern): color(color), pattern(pattern), length(length), debljina(lineWidth)
{
	this->name = new td::String(name);
	setPoints(x, y, length);
}


Function::Function(gui::Point* points, size_t length, td::ColorID color, const td::String& name, double lineWidth, td::LinePattern pattern) : color(color), pattern(pattern), length(length), debljina(lineWidth)
{
	this->name = new td::String(name);
	tacke = new gui::Point[length];
	memcpy(tacke, points, sizeof(gui::Point)*length);
}


Function::Function(Function&& f) noexcept {
	*this = std::move(f);
}

Function& Function::operator=(Function&& f) noexcept{
	memcpy(this, &f, sizeof(Function));
	f.name = nullptr;
	f.tacke = nullptr;
	return *this;

}

void Function::getScale(gui::CoordType& scaleXX, gui::CoordType& scaleYY) const{
	scaleXX = this->scaleX;
	scaleYY = this->scaleY;
}

void Function::getShift(gui::CoordType& shiftX, gui::CoordType& shiftY) const{
	shiftX = this->shiftX;
	shiftY = this->shiftY;
}


void Function::setPoints(gui::CoordType* x, gui::CoordType* y, size_t length){
	delete[] tacke;
	tacke = new gui::Point[length];

	for (size_t i = 0; i < length; ++i){
		tacke[i].x = x[i];
		tacke[i].y = y[i];
	}

}


void Function::increaseScaleAndShiftX(const gui::CoordType& scale, const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].x = (tacke[i].x - shiftX) * scale + shiftX + shift;

	scaleX *= scale;
	shiftX += shift;
	
}

void Function::increaseScaleAndShiftY(const gui::CoordType& scale, const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].y = (tacke[i].y + shiftY) * scale - shiftY - shift;

	scaleY *= scale;
	shiftY += shift;
}

void Function::increaseShiftX(const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].x += shift;

	shiftX += shift;
}

void Function::increaseShiftY(const gui::CoordType& shift){
	for (size_t i = 0; i < length; ++i)
		tacke[i].y -= shift;

	shiftY += shift;
}

void Function::increaseScaleX(const gui::CoordType& scale){
	for (size_t i = 0; i < length; ++i)
		tacke[i].x = (tacke[i].x - shiftX) * scale + shiftX;
	scaleX *= scale;
}

void Function::increaseScaleY(const gui::CoordType& scale) {
	for (size_t i = 0; i < length; ++i)
		tacke[i].y = (tacke[i].y + shiftY) * scale - shiftY;
	scaleY *= scale;
}

void Function::draw(const gui::Rect& frame){
	if (length == 0)
		return;

	if (frame.isZero()) {
		for (size_t i = 0; i < length - 1; ++i)
			gui::Shape::drawLine(tacke[i], tacke[i+1], color, debljina, pattern);
		return;
	}
	
	if (pattern == td::LinePattern::NA) {
		gui::Shape dot;
		std::vector<gui::Circle> dots;
		dots.reserve(length);
		for (size_t i = 1; i < length; ++i) 
			if (frame.contains(tacke[i])) 
				dots.emplace_back(tacke[i], 1);
			
			dot.createCircles(dots.data(), dots.size(), debljina);
			dot.drawFillAndWire(color, color);
			return;
	}

	bool past = frame.contains(tacke[0]) ? true : false;

	for (size_t i = 1; i < length; ++i) {
		if (frame.contains(tacke[i])) {
			if (!past) 
				gui::Shape::drawLine(findIntersection(tacke[i - 1], tacke[i], frame), tacke[i], color, debljina, pattern);
			else
				gui::Shape::drawLine(tacke[i - 1], tacke[i], color, debljina, pattern);

			past = true;
			continue;
		}

		if (!past) { // ako dvije tacke zaredom nisu unutar prozora provjeri da li njihov spoj se sudara sa prozorom
			
			gui::Point p1 = findIntersection(tacke[i - 1], tacke[i], frame);
			if(p1.x >= frame.left && p1.x <= frame.right)
				if (p1.y <= frame.bottom && p1.y >= frame.top)
				gui::Shape::drawLine(p1, findIntersection(tacke[i], tacke[i-1], frame), color, debljina, pattern);
			continue;
		}
		past = false;

		gui::Shape::drawLine(tacke[i-1], findIntersection(tacke[i - 1], tacke[i], frame), color, debljina, pattern);
	}

}



