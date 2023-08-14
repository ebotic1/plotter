#pragma once
#include "gui/SplitterLayout.h"
#include "gui/PropertyEditorSwitcher.h"
#include "gui/View.h"
#include "canvas.h"

#include "gui/TextEdit.h"
#include "../../common/property.h"

#include "gui/HorizontalLayout.h"
#include "gui/LineEdit.h"
#include "gui/Label.h"
#include "gui/NumericEdit.h"
#include "gui/TextEdit.h"
class property : public gui::HorizontalLayout {
	gui::Label p_name;
	gui::Control* edit;

	inline void append(Control& ctrl, td::HAlignment hAlign = td::HAlignment::Center, td::VAlignment vAlign = td::VAlignment::Center) { ((gui::HorizontalLayout*)this)->append(ctrl, hAlign, vAlign); }
	StackedLayout& operator << (Control& ctrl) = delete;
	void appendLayout(Layout& layout) = delete;
	StackedLayout& operator << (Layout& layout) = delete;

public:
	property(const td::String &name, td::DataType type, const td::String &tooltip = "", td::Variant defaultValue = td::Variant(td::DataType::TD_NONE)) : gui::HorizontalLayout(2), p_name(name) {
		append(p_name);
		td::Variant v(type);
		bool numeric = v.isNumeric();
		if (numeric)
			edit = new gui::NumericEdit(type, gui::LineEdit::Messages::Send, true, tooltip);
		else
			edit = new gui::LineEdit(tooltip, gui::LineEdit::Messages::Send);

		if (defaultValue.getType() != td::DataType::TD_NONE)
			if (numeric)
				((gui::NumericEdit*)edit)->setValue(defaultValue);
			else
				((gui::LineEdit*)edit)->setValue(defaultValue);

		gui::Size sz;
		edit->getSize(sz);
		edit->setSize(gui::Size(100, sz.height)); // naknadno postavljanje velicine ne radi (dok za label radi)
		append(*edit);

		

	}


	void setLabelMinSize(int width) {
		gui::Size sz;
		p_name.getSize(sz);
		if (sz.width < width) {
			p_name.setSize(gui::Size(width, sz.height));
		}
		this->reMeasure(gui::CellInfo());
	}

	
	virtual bool onFinishEdit(gui::LineEdit* pCtrl) override{//nikad se ne poziva???

		return true;
	}


};

class modelSettings : public gui::View {
	gui::VerticalLayout v;
	property name;
	

public:
	modelSettings() : v(1), name("model name", td::DataType::string8, "name of the model that will be generated") {
		v.appendLayout(name);
		setLayout(&v);

	}

};

class blockSettings : public gui::View {
	property num, dem;
public:
	blockSettings(): num("numerator: ", td::string8, "Numerator of the transfer function (use 's' as a variable)", td::Variant(td::string8, 1)), 
		dem("denominator: ", td::string8, "Denominator of the transfer function (use 's' as a variable)")
	{

	}
};

class properties : public gui::ViewSwitcher {
	modelSettings modSettings;
	blockSettings bSettings;
public:
	properties() {

	}
};


class mainView : public gui::View {
	gui::SplitterLayout spliter;
	kanvas _canvas;

	properties props;


public:
	mainView(): spliter(gui::SplitterLayout::Orientation::Horizontal) {

		spliter.setContent(_canvas, props);

		
		
		
		setLayout(&spliter);
	}




};

