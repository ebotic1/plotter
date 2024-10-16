#pragma once
#include <gui/View.h>
#include <vector>
#include <gui/GridLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/TextEdit.h>
#include <gui/ViewScroller.h>
#include <gui/GridComposer.h>
#include <gui/DrawableString.h>
#include <gui/Label.h>
#include <cnt/StringBuilder.h>
#include <gui/Canvas.h>

#include "dataDrawer.h"





class NumberColumn : public gui::Canvas{
	gui::DrawableString numbersStr;
	gui::Font::ID font = gui::Font::ID::SystemNormal;
	gui::Size sz;
	bool textSet = false;
public:
	NumberColumn(){

	}

	void setText(const td::String &text){
		numbersStr = text;
		numbersStr.measure(font, sz);
		textSet = true;
	}

	void setNumbers(double *numbers, unsigned int size){
		cnt::StringBuilder str;
		for(int i = 0; i<size; ++i){
			str << numbers[i] << "\n";
		}
		td::String temp;
		str.getString(temp);
		setText(temp);
	}


	void onDraw(const gui::Rect& rect){
		if(textSet)
			numbersStr.draw({0,0}, font, td::ColorID::SysText);
	}

	gui::Size getSize(){
		return sz;
	}

};

static inline void setSizeToBigger(gui::Size &initialSize, const gui::Size comparedSize){
	initialSize.width = (initialSize.width > comparedSize.width) ? initialSize.width : comparedSize.width;
	initialSize.height = (initialSize.height > initialSize.height) ? initialSize.height : comparedSize.height;
}

class Table : public gui::ViewScroller {
	std::vector<gui::GridLayout *> layouts;
	std::vector<gui::Label *> labels;
	std::vector<NumberColumn> numberCols;
	gui::HorizontalLayout _hLayout;
	gui::View mainView;
	gui::Size sz = {0,0};

public:
	Table(const std::vector<DataDraw::FunctionDesc> &funcs):
		gui::ViewScroller(gui::ViewScroller::Type::ScrollerAlwaysVisible, gui::ViewScroller::Type::ScrollerAlwaysVisible),
		_hLayout(funcs.size()),
		layouts(funcs.size()),
		labels(funcs.size() * 3),
		numberCols(funcs.size() * 2)
	{
		for (int i = 0; i < funcs.size(); ++i) {
			layouts[i] = new gui::GridLayout(3, 2);

			labels[3 * i] = new gui::Label(funcs[i].name);
			labels[3 * i + 1] = new gui::Label(funcs[i].xname);
			labels[3 * i + 2] = new gui::Label(funcs[i].yname);
			if (funcs[i].name != funcs[i].yname);
				layouts[i]->insert(0, 0, *labels[3 * i], 2, td::HAlignment::Center);

			layouts[i]->insert(1, 0, *labels[3 * i + 1], td::HAlignment::Center);
			layouts[i]->insert(1, 1, *labels[3 * i + 2], td::HAlignment::Center);


			
			
			numberCols[2 * i].setNumbers(funcs[i].x, funcs[i].size);
			numberCols[2 * i + 1].setNumbers(funcs[i].y, funcs[i].size);

			setSizeToBigger(sz,	numberCols[2 * i].getSize());
			setSizeToBigger(sz,	numberCols[2 * i + 1].getSize());

			layouts[i]->insert(2, 0, numberCols[2 * i], td::HAlignment::Center);
			layouts[i]->insert(2, 1, numberCols[2 * i + 1], td::HAlignment::Center);

			_hLayout << *layouts[i];


		}

		mainView.setLayout(&_hLayout);
		setContentView(&mainView);
		setContentSize({400,3000});
		scaleContent(4);
	}

	bool getModelSize(gui::Size& modelSize) const override{
		modelSize.width = 400;
		modelSize.height = 3000;
		return true;
	}

	~Table() {
		for (const auto& ptr : labels)
			delete ptr;
		for (const auto& ptr : layouts)
			delete ptr;

	}
};