#include "blockInterface.h"
#include "tBlock.h"
#include "sumBlock.h"
#include "nlBlock.h"

std::vector<int> blockInterface::cnts;

void blockInterface::saveState()
{
	cnts = {TFBlock::blockCnt, sumBlock::blockCnt, NLBlock::blockCnt};
}

void blockInterface::restoreState()
{
	std::tie(TFBlock::blockCnt, sumBlock::blockCnt, NLBlock::blockCnt) = std::make_tuple(cnts[0], cnts[1], cnts[2]);
}

void blockInterface::writeState(arch::ArchiveOut& f)
{
	f << TFBlock::blockCnt << sumBlock::blockCnt << NLBlock::blockCnt;
}

void blockInterface::reloadState(arch::ArchiveIn& f)
{
	f >> TFBlock::blockCnt >> sumBlock::blockCnt >> NLBlock::blockCnt;
}
