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

class View : public gui::View {
	gui::HorizontalLayout hLayout, opcije;
	gui::HorizontalLayout timeLayout;
	gui::NumericEdit _start, _end, _step;
	gui::Label starLabel, endLabel, korakLabel;

	gui::VerticalLayout vLayout;
	gui::TextEdit _jednacineText;
	gui::Label jednacinaLabel;
	

	gui::HorizontalLayout dugmadi;
	gui::Button _generisiXML, _generisiTXT;



	void pokreniSolver();


public:

	View();
	virtual bool onClick(gui::Button* pBtn);

	
};