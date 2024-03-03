#include "blockBase.h"
#include "globals.h"


const gui::Rect& BlockBase::getRect() const{
	return _r;
}

const bool BlockBase::getInputSwitched() const{
	return switchOutput;
}


const gui::Point BlockBase::getLocation() const{
	return gui::Point(_r.left, _r.top);
}

const std::vector<std::pair<BlockBase*, int>>& BlockBase::getConnectedToBlocks() const
{
	return connectedTo;
}

const std::vector<std::pair<BlockBase*, int>>& BlockBase::getConnectedFromBlocks() const
{
	return connectedFrom;
}




bool BlockBase::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}

int BlockBase::intersectsInput(const gui::Point& p) const
{
	for (int i = 0; i < getInputCnt(); ++i)
		if (gui::Circle(getInput(i), 14).contains(p))
			return i;

	return -1;
}

int BlockBase::intersectsOutput(const gui::Point& p) const
{
	for (int i = 0; i < getOutputCnt(); ++i)
		if (gui::Circle(getOutput(i), 14).contains(p))
			return i;

	return -1;
}


void BlockBase::connectTo(BlockBase* block, int pozFrom, int pozTo) {
	if (block == this)
		return;

	connectedTo.at(pozFrom) = { block, pozTo };
	block->connectedFrom.at(pozTo) = { this, pozFrom };

	if(!disableSetUp)
		setUpWires(true);
}

void BlockBase::disableLogic(bool disable){
	disableSetUp = disable;
}




void BlockBase::removeConnections() {
	disableSetUp = true;
	for (int i = 0; i < getOutputCnt(); ++i)
		disconnectOutput(i);

	for (int i = 0; i < getInputCnt(); ++i)
		disconnectInput(i);

	disableSetUp = false;
	setUpWires(true);
}

void BlockBase::disconnectInput(int poz){
	auto &par = connectedFrom.at(poz);
	if (par.first == nullptr) return;
	par.first->connectedTo.at(par.second).first = nullptr;
	if(!par.first->disableSetUp)
		par.first->setUpWires(false);
	par.first = nullptr;

	if (!disableSetUp)
		setUpWires(false);
}

void BlockBase::disconnectOutput(int poz){
	auto& par = connectedTo.at(poz);
	if (par.first == nullptr) return;
	par.first->connectedFrom.at(par.second).first = nullptr;
	if (!par.first->disableSetUp)
		par.first->setUpWires(false);
	par.first = nullptr;

	if (!disableSetUp)
		setUpWires(false);
}


void BlockBase::switchInput() {
	switchOutput = !switchOutput;
	if (!disableSetUp)
		this->setUpAll();
}

void BlockBase::setPosition(const gui::Point& position){
	_r.setOrigin(position);
	if (!disableSetUp)
		this->setUpAll();
}


gui::View& BlockBase::updateSettingsView(settingsView& view)
{
	return view;
}


BlockBase::BlockBase(const gui::Point& position)
{
	_r.setOrigin(position);
}


BlockBase::~BlockBase(){

}

