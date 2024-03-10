#include "sumBlock.h"
#include "globals.h"

int	sumBlock::blockCnt = 0;

sumBlock::sumBlock(gui::Point position, bool addition_operator, int cnt):
	squareBlockMInameless(cnt),
	BlockBase(position)
{
	changeSign(addition_operator);
	td::String name;
	name.format("sum%d", blockCnt++);
	setOutputName(name);

	setUpAll();

}

void sumBlock::setUpBlock()
{
	if (disableSetUp)
		return;

	_r.setWidth(100);
	squareBlock::setUpBlock();
	if (!znak.isInitialized())
		changeSign(sumOperator);

	squareBlockSO::setUpBlock();



}


void sumBlock::drawBlock(td::ColorID color)
{
	squareBlock::drawBlock(color);
	znak.draw(_r, &blockFont, color, td::TextAlignment::Center, td::VAlignment::Center);


	squareBlockSO::drawBlock(color);
	
}

void sumBlock::writeToModel(modelNode& model, Nodes& nodes)
{
	static constexpr Nodes::name pop_nodes[] = { Nodes::name::var, Nodes::name::nl};
	BlockBase::populateNodes(pop_nodes, 2, model, nodes);

	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String outputName = getOutputName(0);

	var.processCommands(outputName);
	if (!getIsConnectedTo())
		var.nodes.back()->attribs["out"] = "true";

	outputName = outputName.subStr(0, outputName.find("=") - 1);

	char znak = sumOperator ? '+' : '-';

	cnt::StringBuilder cmnd;
	cmnd << outputName << " = ";
	bool first = true;
	for (const auto& par : getConnectedFromBlocks()) 
		if (par.first != nullptr) {
			if (!first) 
				cmnd << " " << znak << " ";
			cmnd << par.first->getOutputName(par.second);
			first = false;
		}

	if (!first)
		nle.processCommands(cmnd.toString());
	else {
		cmnd << "0";
		nle.processCommands(cmnd.toString());
	}
	


}

void sumBlock::saveToFile(arch::ArchiveOut& f)
{
	f << sumBlock::getID() << getLocation().x << getLocation().y << sumOperator << getInputCnt() << switchOutput << izlazName;
}

sumBlock* sumBlock::restoreFromFile(arch::ArchiveIn& f)
{
	gui::Point p;
	bool opr;
	int cnt;
	f >> p.x >> p.y >> opr >> cnt;
	auto pok = new sumBlock(p, opr, cnt);
	f >> opr;
	if(opr)
		pok->switchInput();

	f >> pok->izlazName;

	return pok;
}

void sumBlock::updateSettingsView(BlockBase::settingsView* view)
{
	squareBlockMInameless::updateSettingsView(view);
	squareBlockSO::updateSettingsView(view);
	BlockBase::updateSettingsView(view);
	auto pogled = (settingsView*)view;
	pogled->currentBlock = this;
	pogled->izbor.selectIndex(sumOperator);
}

void sumBlock::changeSign(bool addition)
{
	sumOperator = addition;
	znak = sumOperator ? "+" : "-";
	setUpBlock();
	globals::refreshCanvas();
}

sumBlock::~sumBlock()
{
	disableSetUp = true;
	removeConnections();
}
