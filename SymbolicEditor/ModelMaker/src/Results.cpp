#include "Results.h"

Results::Results()
: _layout(1)
{
    _layout << _table;
	setLayout(&_layout);
}

dp::IDataSet* Results::getDataSet()
{
    return _table.getDataSet();
}

//void Results::refresh()
//{
//    _table.reload();
//    _table.selectRow(0, true);
//}

void Results::show(dp::IDataSet* pDS)
{
    dp::IDataSet* currDataSet = getDataSet();
    if (currDataSet == nullptr || currDataSet != pDS)
    {
        _table.init(pDS, 0);
    }
    else //if (currDataSet == pDS)
    {
        _table.reload();
    }
    
}
