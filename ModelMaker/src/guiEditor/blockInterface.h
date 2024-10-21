#pragma once
#include <vector>
#include "arch/ArchiveIn.h"
#include "arch/ArchiveOut.h"

class blockInterface {
	static std::vector<int> cnts;
public:
	static void saveState();
	static void restoreState();
	static void writeState(arch::ArchiveOut &f);
	static void reloadState(arch::ArchiveIn& f);
};