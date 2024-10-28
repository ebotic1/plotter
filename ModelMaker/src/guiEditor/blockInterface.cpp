#include "blockInterface.h"
//#include "tBlock.h"
//#include "sumBlock.h"
//#include "nlBlock.h"



void blockInterface::saveState()
{
	_cntsSaved = cnts;
}

void blockInterface::restoreState()
{
	cnts = _cntsSaved;
}

void blockInterface::writeState(arch::ArchiveOut& f)
{
	f << cnts.size();
	for (const auto& c : cnts)
		f << c.first << c.second;
}

void blockInterface::reloadState(arch::ArchiveIn& f)
{
	int size;
	f >> size;
	size_t hash;
	for (int i = 0; i < size; ++i) {
		f >> hash;
		f >> cnts[hash];
	}
		
}

void blockInterface::resetAll()
{
	cnts.clear();
	_cntsSaved.clear();
}
