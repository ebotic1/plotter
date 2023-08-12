#include "gui/SplitterLayout.h"
#include "gui/PropertyEditorSwitcher.h"
#include "gui/View.h"
#include "canvas.h"




class mainView : public gui::View {
	gui::SplitterLayout spliter;
	kanvas _canvas;



public:
	mainView(): spliter(gui::SplitterLayout::Orientation::Horizontal) {
		_canvas.setBackgroundColor(td::ColorID::Black);

		//spliter.setContent(_canvas, _sm);
			
		setLayout(&spliter);
	}
};

