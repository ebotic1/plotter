#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>

class LogView : public gui::View {

	gui::TextEdit textMain;
	gui::VerticalLayout _vl;

public:

	LogView() :
		_vl(1)
	{
		textMain.setAsReadOnly();
		_vl << textMain;
		setLayout(&_vl);
	}

};