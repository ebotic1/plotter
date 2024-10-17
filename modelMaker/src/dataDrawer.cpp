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
		td::String xname = functions[0].xname, yname;
		int ynameCnt = 0;

		for (const auto& f : functions) {

			if (f.x != nullptr && f.y == nullptr && f.size == 1) { //param u x
				g->verticals.emplace_back(*f.x);
				continue;
			}

			if (f.y != nullptr && f.x == nullptr && f.size == 1) { //param u y
				g->horizontals.emplace_back(*f.y);
				continue;
			}

			if (f.x != nullptr && f.y != nullptr) { //dva niza
				xname = f.xname;
				if (ynameCnt++ == 0)
					yname = f.yname;
				g->addFunction(f.x, f.y, f.size, 2, td::LinePattern::Solid, f.name);
				if (f.xname != xname)
					xname.clean();
			}

		}

		if (!xname.isNull())
			g->setxAxisName(xname);
		if (ynameCnt == 1)
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

void DataDraw::removeTabs()
{
	for (int i = 0; i < tabView.getNumberOfViews(); ++i) 
		tabView.removeAll();
}

DataDraw::~DataDraw()
{
	int x = 0;
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
