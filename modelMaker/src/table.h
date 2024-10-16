#pragma once
#include <gui/View.h>
#include <vector>
#include <gui/GridLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/TextEdit.h>
#include <gui/ViewScroller.h>
#include <gui/GridComposer.h>
#include <gui/Label.h>
#include <cnt/StringBuilder.h>

#include "dataDrawer.h"


static void setUpTextEdit(gui::TextEdit& text) {
	text.setFlat();
	text.setAsReadOnly();
	text.setSizeLimits(60, gui::Control::Limit::Fixed);
	text.setBackgroundColor(td::ColorID::LightGray);
}


class Table : public gui::ViewScroller {
	std::vector<gui::GridLayout *> layouts;
	std::vector<gui::Label *> labels;
	std::vector<gui::TextEdit> texts;
	gui::HorizontalLayout _hLayout;
	gui::View mainView;

public:
	Table(const std::vector<DataDraw::FunctionDesc> &funcs):
		gui::ViewScroller(gui::ViewScroller::Type::ScrollerAlwaysVisible, gui::ViewScroller::Type::ScrollerAlwaysVisible),
		_hLayout(funcs.size()),
		layouts(funcs.size()),
		labels(funcs.size() * 3),
		texts(funcs.size() * 2)
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


			setUpTextEdit(texts[2 * i]);
			setUpTextEdit(texts[2 * i + 1]);
			

			const auto fun = [&funcs, i](double *data, gui::TextEdit *textEdit) {
				cnt::StringBuilder str;
				int size = funcs[i].size;
				for (int j = 0; j < size; ++j)
					str << data[j] << "\n";
				td::String text;
				str.getString(text);
				textEdit->setText(text);
			};
			
			fun(funcs[i].x, &texts[2 * i]);
			fun(funcs[i].y, &texts[2 * i + 1]);

			layouts[i]->insert(2, 0, texts[2 * i], td::HAlignment::Center);
			layouts[i]->insert(2, 1, texts[2 * i + 1], td::HAlignment::Center);

			_hLayout << *layouts[i];
		}

		mainView.setLayout(&_hLayout);
		setContentView(&mainView);
	}

	~Table() {
		for (const auto& ptr : labels)
			delete ptr;
		for (const auto& ptr : layouts)
			delete ptr;

	}
};