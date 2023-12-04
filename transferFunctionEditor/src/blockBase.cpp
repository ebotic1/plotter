#include "blockBase.h"
#include "globals.h"

const gui::Rect& BlockBase::getRect() const{
	return _r;
}

const bool BlockBase::getInputSwitched() const{
	return switchOutput;
}


const gui::Point & BlockBase::getOutput() const{
	return outputPoint;
}


const gui::Point& BlockBase::getLocation() const{
	return gui::Point(_r.left, _r.top);
}


const std::set<BlockBase*>& BlockBase::getConnectedToBlocks() const{
	return connectedTo;
}

const std::vector<std::set<BlockBase*>>& BlockBase::getConnectedFromBlocks() const{
	return connectedFrom;
}



bool BlockBase::intersectsBlock(const gui::Point& p) {
	return _r.contains(p);
}

bool BlockBase::intersectsOutput(const gui::Point& p){
	return gui::Circle(outputPoint, 14).contains(p); 
}


void BlockBase::connectTo(BlockBase* block, int poz) {
	if (block == this)
		return;

	if (block->connectedFrom.size() < poz+1)
		block->connectedFrom.resize(poz+1);

	block->connectedFrom[poz].insert(this);
	connectedTo.insert(block);
	setUpWires(true);
}

void BlockBase::disableLogic(bool disable){
	disableSetUp = disable;
}

void BlockBase::removeConnections() {
	for(BlockBase*var : connectedTo){
		for(std::set<BlockBase*> &block : var->connectedFrom)
			block.erase(this);
		

	}
	connectedTo.clear();
	for(std::set<BlockBase*> &var : connectedFrom)
		for (BlockBase* block : var) {
			block->connectedTo.erase(this);
			block->setUpWires(false);
		}
	
	connectedFrom.clear();
	setUpWires(true);
}


void BlockBase::switchInput() {
	switchOutput = !switchOutput;
	this->setUpAll();
}

void BlockBase::setPosition(const gui::Point& position){
	_r.setOrigin(position);
	this->setUpAll();
}


BlockBase::BlockBase(const gui::Point& position){
	_r.setOrigin(position);
}


BlockBase::~BlockBase(){

}