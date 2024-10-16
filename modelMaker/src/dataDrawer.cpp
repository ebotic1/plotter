#include "dataDrawer.h"
#include "table.h"



DataDraw::FunctionDesc::FunctionDesc(const td::String& name, double* x, double* y, unsigned int size, const td::String& xname, const td::String& yname):
	name(name),
	x(x),
	y(y),
	size(size),
	xname(xname),
	yname(yname)
{
}



DataDraw::DataDraw():
	imgGraph(":txtIcon"),
	_hl(1),
	tabView(gui::TabHeader::Type::Dynamic, 10, 50)
{
	_hl << tabView;
	setLayout(&_hl);



}

void DataDraw::measure(gui::CellInfo& cell)
{
	gui::View::measure(cell);
	//cell.nResHor = 1;
	//cell.nResVer = 1;
	cell.minHor = 500;
}



void DataDraw::addData(const td::String& name, const std::vector<FunctionDesc>& functions, Type tip)
{
	gui::BaseView* tab = nullptr;
	if (functions.size() == 0)
		return;

	if (tip == Type::graph) {
		auto g = new graph(true, true, td::ColorID::SysCtrlBack);
		td::String xname = functions[0].xname;

		for (const auto& f : functions) {
			g->addFunction(f.x, f.y, f.size, 2, td::LinePattern::Solid, f.name);
			if (f.xname != xname)
				xname.clean();
		}

		if (!xname.isNull())
			g->setxAxisName(xname);
		if (functions.size() == 1)
			g->setyAxisName(functions[0].yname);

		tab = g;
	}
	else {
		auto table = new Table(functions);
		tab = table;
	}
	

	for (int i = 0; i < tabView.getNumberOfViews(); ++i) 
		if (((Tab*)tabView.getView(i))->name == name) {
			tabView.removeView(i);
			break;
		}
	

	tabView.addView(new Tab(name, tab), name, &imgGraph);
	
}

DataDraw::Tab::Tab(const td::String& name, gui::BaseView* view):
	name(name),
	view(view),
	gl(1,1)
{
	gl.insert(0, 0, *view);
	setLayout(&gl);
}

DataDraw::Tab::~Tab()
{
	delete view;
}
