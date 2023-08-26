#include "gui/SplitterLayout.h"
#include "gui/View.h"
#include "./../../Canvas/inc/canvas.h"
#include "gui/ComboBox.h"
#include "gui/VerticalLayout.h"

class switcher;



class splitterLayout : public gui::View {
	gui::SplitterLayout _mainLayout;

	gui::ComboBox picker;
	gui::VerticalLayout v;
	switcher* props = nullptr;
	graph& _graph;

	bool ignoreSelections = false;

public:
	splitterLayout(graph& mainView);
	void refreshPicks();


	bool onChangedSelection(gui::ComboBox* pCmb) override;

	~splitterLayout() {
		delete props;
	}
	
};