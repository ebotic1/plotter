#include "splitter.h"

#include "gui/VerticalLayout.h"
#include "gui/ComboBox.h"
#include "gui/Slider.h"
#include "gui/ColorPicker.h"
#include "gui/ViewSwitcher.h"
#include "./../../common/property.h"
#include "./../../Canvas/inc/canvas.h"


class switcher : public gui::ViewSwitcher {
	gui::VerticalLayout graphProps;
	gui::VerticalLayout lineProps;
	gui::View view1, view2;
	graph& mainGraph;

	elementProperty name;
	gui::Slider slider; gui::Label sliderLabel;
	gui::ColorPicker line_color;
	gui::ComboBox pattern;

	
public:
	switcher(graph& mainView) : gui::ViewSwitcher(2), graphProps(1), lineProps(5), mainGraph(mainView),
		name("name: ", td::string8, "name of the function that appears on the legend"),
		sliderLabel("Line width:")
	{
		
		lineProps << name << sliderLabel << slider << line_color << pattern;

		view1.setLayout(&lineProps);
		view1.forwardMessagesTo(this);
		view2.setLayout(&graphProps);
		view2.forwardMessagesTo(this);
		addView(&view1, false);
		addView(&view2, false);
	}

	bool onChangedValue(gui::Slider* pSlider) override;
	
	void showFunction(size_t pos);

};

bool switcher::onChangedValue(gui::Slider* pSlider){



	return false;
}

void switcher::showFunction(size_t pos){
	showView(2, false);
}


class pickerView : public gui::View {
	gui::VerticalLayout v;
	gui::ComboBox picker;
public:
	pickerView(): v(1) {
		v << picker;
		setLayout(&v);
	}
};


splitterLayout::splitterLayout(graph& mainView) : gui::SplitterLayout(gui::SplitterLayout::Orientation::Horizontal), v(4), _graph(mainView){
	props = new switcher(mainView);
	
	v << picker;
	v.appendSpace(30);
	v << *props;
	v.appendSpacer();
	
	setContent(mainView, v);

}

void splitterLayout::refreshPicks(){
	picker.clean();
	picker.addItem("Graph");
	auto &funs = _graph.getFunctions();
	for (const Function& fun : funs) {
		picker.addItem(fun.name->c_str());
	}
	picker.selectIndex(0);
	
}

bool splitterLayout::onChangedSelection(gui::ComboBox* pCmb){

	if (pCmb->getSelectedIndex() == 0) {
		props->showView(0, false);
		return true;
	}

	props->showFunction(pCmb->getSelectedIndex() - 1);

	return true;
}
