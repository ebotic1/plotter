#include "gui/View.h"
#include "gui/TextEdit.h"
#include "gui/VerticalLayout.h"
#include "gui/HorizontalLayout.h"
#include "gui/Label.h"
#include "gui/Button.h"
#include "td/Variant.h"
#include <vector>
#include "gui/ComboBox.h"
#include "gui/NumericEdit.h"
#include "../../Canvas/inc/canvas.h"
#include "gui/SplitterLayout.h"

class View : public gui::View {
	gui::HorizontalLayout opcije;
	gui::HorizontalLayout timeLayout;
	gui::NumericEdit _start, _end, _step;
	gui::Label starLabel, endLabel, korakLabel;

	gui::VerticalLayout vLayout;
	gui::TextEdit _jednacineText;
	gui::Label jednacinaLabel;
	

	gui::HorizontalLayout dugmadi;
	gui::Button _generisiTXT;


	void pokreniSolver();
	td::String currentPath, currentPathXML;
	
	graph plotter;
	gui::SplitterLayout splitter;


public:

	View();
	virtual bool onClick(gui::Button* pBtn);
	bool loadFile(const td::String &path);
	bool save();
	bool saveAs(td::String path);
	
};