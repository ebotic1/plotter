#pragma once
#include <gui/View.h>
#include <gui/Window.h>
#include <gui/TabView.h>
#include <gui/TableEdit.h>
#include <gui/HorizontalLayout.h>
#include <gui/Application.h>
#include <td/String.h>
#include <gui/Image.h>
#include <gui/GridLayout.h>
#include <map>

#include <gui/plot/View.h>


class DataDraw : public gui::View 
{
	gui::TabView *_tabView;
	gui::HorizontalLayout _hl;
	gui::Image imgGraph, imgTable;
	bool _tabViewOwnership;
	std::map<gui::BaseView *, td::String> _graphNames;


public:
	struct FunctionDesc {
		td::String name, xname, yname;
		double* x, * y;
		unsigned int size;
		FunctionDesc(const td::String& name, double* x, double* y, unsigned int size, const td::String& xname, const td::String& yname);
	};

	enum class Type{Table, Graph};

	DataDraw(gui::TabView* tabView = nullptr);
	void measure(gui::CellInfo& cell) override;
	void addData(const td::String &name, const std::vector<FunctionDesc>& functions, Type tip);
	void removeTabs();
	~DataDraw();
};
