#include "DataDrawer.h"
#include "Table.h"
#include "GlobalEvents.h"

DataDraw::FunctionDesc::FunctionDesc(const td::String& name, double* x, double* y, unsigned int size, const td::String& xname, const td::String& yname):
	name(name),
	x(x),
	y(y),
	size(size),
	xname(xname),
	yname(yname)
{
}

DataDraw::DataDraw(gui::TabView* tabView):
	imgGraph(":graphData"),
	imgTable(":tableData"),
	_hl(1),
	_tabViewOwnership(tabView == nullptr),
	_tabView((tabView == nullptr) ? new gui::TabView(gui::TabHeader::Type::Dynamic, 10, 50) : tabView)
{
	if (_tabViewOwnership) {
		_hl << *_tabView;
		setLayout(&_hl);
	}
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
    gui::Image *img;
    if (functions.size() == 0)
        return;

    if (tip == Type::Graph)
    {
        static gui::Font *f = new gui::Font();
        static bool init = false;
        if(!init){
            init = true;
            f->create(GlobalEvents::settingsVars.font, 10, gui::Font::Style::Normal, gui::Font::Unit::Point);
        }
        auto g = new gui::plot::View(f, f, td::ColorID::SysCtrlBack, false);
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
                g->addFunction(f.x, f.y, f.size, 2, gui::plot::View::LinePattern::Default, f.name);
                if (f.xname != xname)
                    xname.clean();
            }

        }

        if (!xname.isNull())
            g->setxAxisName(xname);
        if (ynameCnt == 1)
            g->setyAxisName(functions[0].yname);

        g->fitToWindow();
        tab = g;
        img = &imgGraph;
        
    }
    else
    {
        auto table = new Table(functions);
        tab = table;
        img = &imgTable;
    }
    
        decltype(_graphNames)::iterator it;
        for (int i = 0; i < _tabView->getNumberOfViews(); ++i){
            it = _graphNames.find(_tabView->getView(i));
            if(it == _graphNames.end())
                continue;

            if (it->second == name) {
                _tabView->removeView(i);
                if(_tabView->getNumberOfViews() == 0)
                    _graphNames.clear();
                break;
            }
        }
    
    
    _graphNames[tab] = name;
    _tabView->addView(tab, name, img, (td::BYTE) DocumentType::Graph);
}

void DataDraw::removeTabs()
{
	for (int i = 0; i < _tabView->getNumberOfViews(); ++i) 
		_tabView->removeAll();
    _graphNames.clear();
}

DataDraw::~DataDraw()
{
	if(_tabViewOwnership)
		delete _tabView;
}


