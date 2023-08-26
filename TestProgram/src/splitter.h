#include "gui/SplitterLayout.h"
#include "gui/View.h"
#include "./../../Canvas/inc/canvas.h"
#include "gui/ComboBox.h"
#include "gui/VerticalLayout.h"

class switcher;

class splitterLayout : public gui::SplitterLayout {


	gui::VerticalLayout v;
	switcher* props = nullptr;
	graph& _graph;
	
	

public:
	splitterLayout(graph& mainView);
	void refreshPicks();


	bool onChangedSelection(gui::ComboBox* pCmb) override;

	~splitterLayout() {
		delete props;
	}
};