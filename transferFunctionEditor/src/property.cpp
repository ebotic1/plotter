#pragma once
#include "property.h"

inline void elementProperty::append(Control& ctrl, td::HAlignment hAlign, td::VAlignment vAlign) {
	((gui::HorizontalLayout*)this)->append(ctrl, hAlign, vAlign);
}

elementProperty::elementProperty(const td::String& name, td::DataType type, const td::String& tooltip, td::Variant defaultValue) : gui::HorizontalLayout(2), p_name(name) {
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

void elementProperty::setLabelMinSize(int width) {
	gui::Size sz;
	p_name.getSize(sz);
	if (sz.width < width) {
		p_name.setSize(gui::Size(width, sz.height));
	}
	this->reMeasure(gui::CellInfo());
}