#include "gui/HorizontalLayout.h"
#include "gui/LineEdit.h"
#include "gui/Label.h"
#include "gui/NumericEdit.h"


class property : public gui::HorizontalLayout {
	gui::Label p_name;
	gui::Control* edit;

	inline void append(Control& ctrl, td::HAlignment hAlign = td::HAlignment::Center, td::VAlignment vAlign = td::VAlignment::Center);
	StackedLayout& operator << (Control& ctrl) = delete;
	void appendLayout(Layout& layout) = delete;
	StackedLayout& operator << (Layout& layout) = delete;

public:
	property(const td::String& name, td::DataType type, const td::String& tooltip = "", td::Variant defaultValue = td::Variant(td::DataType::TD_NONE));

	void setLabelMinSize(int width);

};

