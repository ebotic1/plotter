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

#include "../../Graph/inc/Graph.h"


class DataDraw : public gui::View {
	gui::TabView *_tabView;
	gui::HorizontalLayout _hl;
	gui::Image imgGraph, imgTable;
	bool _tabViewOwnership;

	struct Tab : public gui::View {
		td::String name;
		gui::BaseView* view;
		gui::GridLayout gl;
		Tab(const td::String& name, gui::BaseView *);
		virtual ~Tab();
	};

public:
	struct FunctionDesc {
		td::String name, xname, yname;
		double* x, * y;
		unsigned int size;
		FunctionDesc(const td::String& name, double* x, double* y, unsigned int size, const td::String& xname, const td::String& yname);
	};

	enum class Type{table, graph};

	DataDraw(gui::TabView* tabView = nullptr);
	void measure(gui::CellInfo& cell) override;
	void addData(const td::String &name, const std::vector<FunctionDesc>& functions, Type tip);
	void removeTabs();
	~DataDraw();
};
