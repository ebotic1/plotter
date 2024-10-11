#include "tBlock.h"
#include "globals.h"

int TFBlock::blockCnt = 0;


void TFBlock::setNumerator(const td::String& nominator){
	nom = nominator.isNull() ? "1" : nominator;
	drawNom = nom;
	setUpAll();
}


void TFBlock::setDenominator(const td::String& denominator){
	dem = denominator.isNull() ? "1" : denominator;
	drawDem = dem;
	setUpAll();
}






void TFBlock::setUpBlock()
{

	if (disableSetUp)
		return;

	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		gui::Size s1, s2;
		drawNom.measure(FONT_ID, s1);
		drawDem.measure(FONT_ID, s2);

		gui::CoordType& bigWidth = (s1.width > s2.width) ? s1.width : s2.width;

		_r.setWidth(100 + bigWidth);
		squareBlock::setUpBlock();

		constexpr int offset = 6;

		rectangleNominator.setOrigin({ _r.left, _r.center().y - offset - s1.height });
		rectangleNominator.setSize({ _r.width(), 0 });

		rectangleDenominator.setOrigin({ _r.left, _r.center().y + offset });
		rectangleDenominator.setSize({ _r.width(), 0 });

		gui::Point p[] = { {_r.center().x - bigWidth / 2 - offset, _r.center().y}, {_r.center().x + bigWidth / 2 + offset, _r.center().y} };
		fracLine.createLines(p, 2, 1);

	}
	else {
		drawNom = nom;
		drawDem = dem;
		setUpBlock();
		return;
	}



	squareBlockSO::setUpBlock();
	squareBlockSI::setUpBlock();



	
}



void TFBlock::drawBlock(td::ColorID color) {
	
	if (drawNom.isInitialized() && drawDem.isInitialized()) {
		drawNom.draw(rectangleNominator, &blockFont, color, td::TextAlignment::Center);
		drawDem.draw(rectangleDenominator, &blockFont, color, td::TextAlignment::Center);
		fracLine.drawWire(color, 2);
	}



	squareBlock::drawBlock(color);
	squareBlockSO::drawBlock(color);
	squareBlockSI::drawBlock(color);
}


bool TFBlock::hasLaplaceOperator(const td::String& s)
{
	int poz = 0;

	while (true) {
		poz = s.find("s", poz);
		if (poz == -1)
			break;

		char c = (s.length() > poz + 1) ? s.getAt(poz + 1) : '*';
		if (std::isdigit(c) || c == '*' || c == '/' || c == '+' || c == '-' || c == ' ' || c == '^') {
			c = (poz > 0) ? s.getAt(poz - 1) : c = '*';
			if (c == '*' || c == '/' || c == '+' || c == '-' || c == ' ')
				return true;
		}
		++poz;
	}
	return false;
}

TFBlock::TFBlock(): BlockBase({0,0}) {
	disableSetUp = true;
	blockCnt;
}

TFBlock::TFBlock(const gui::Point& position, const td::String& inputName, const td::String& outputName):
	squareBlockSI(inputName),
	squareBlockSO(outputName),
	BlockBase::BlockBase(position)
{
	setNumerator("1");
	setDenominator("s");

	setInputName(inputName);
	setOutputName(outputName);

	setUpAll();
	globals::refreshCanvas();
}

TFBlock::TFBlock(const gui::Point& position):
	BlockBase::BlockBase(position)
{
	td::String ul, izl;
	ul.format("tf%d_in", blockCnt);
	izl.format("tf%d_out", blockCnt);
	disableSetUp = true;
	setInputName(ul);
	disableSetUp = false;
	setOutputName(izl);
	++blockCnt;


	setNumerator("1");
	setDenominator("s");
	setUpAll();
	globals::refreshCanvas();
}


TFBlock::~TFBlock(){
	disableSetUp = true;
	removeConnections();
}

td::String TFBlock::getID()
{
	return "TFBlock";
}
void TFBlock::saveToFile(arch::ArchiveOut& f)
{
	f << TFBlock::getID() << getLocation().x << getLocation().y << getInputName(0) << getOutputName(0) << nom << dem << switchOutput;
}

TFBlock* TFBlock::restoreFromFile(arch::ArchiveIn& f)
{
	TFBlock* block = new TFBlock();
	f >> block->_r.left >> block->_r.top >> block->ulazName >> block->izlazName;
	f >> block->nom >> block->dem >> block->switchOutput;
	return block;
}

void TFBlock::updateSettingsView(BlockBase::settingsView* view)
{
	BlockBase::updateSettingsView(view);
	squareBlockSI::updateSettingsView(view);
	squareBlockSO::updateSettingsView(view);

	auto viewTf = dynamic_cast<settingsView*>(view);
	if (viewTf == nullptr) return;

	viewTf->currentBlock = this;
	viewTf->num.setValue(nom);
	viewTf->dem.setValue(dem);
}

void TFBlock::writeToModel(modelNode& model, Nodes& nodes)
{
	static constexpr Nodes::name pop_nodes[] = {Nodes::name::var, Nodes::name::nl, Nodes::name::tf};
	BlockBase::populateNodes(pop_nodes, 3, model, nodes);
	
	auto& var = *nodes.nodes[(int)Nodes::name::var];
	auto& tfs = *nodes.nodes[(int)Nodes::name::tf];
	auto& nle = *nodes.nodes[(int)Nodes::name::nl];

	td::String inputName = getInputName(0);
	td::String outputName = getOutputName(0);

	bool isConnectedFrom = getIsConnectedFrom();

	var.processCommands(inputName);
	if (!isConnectedFrom) {
		var.getNodes().back()->attribs["out"] = "true";

		if (inputName.find('=') != -1)
			nle.processCommands(inputName);
	}

	var.processCommands(outputName);
	if (!getIsConnectedTo())
		var.getNodes().back()->attribs["out"] = "true";

	inputName = inputName.subStr(0, inputName.find("=") - 1);
	outputName = outputName.subStr(0, outputName.find("=") - 1);
	cnt::StringBuilder cmnd;
	


	if (isConnectedFrom) {
		cmnd << inputName << " = " << getConnectedFromBlocks()[0].first->getOutputName(getConnectedFromBlocks()[0].second);
		nle.processCommands(cmnd.toString());
		cmnd.reset();
	}

	if (hasLaplaceOperator(nom) || hasLaplaceOperator(dem)) {
		cmnd << outputName << "/" << inputName << " = " << "(" << nom << ")" << "/(" << dem << ")";
		tfs.processCommands(cmnd.toString());
	}
	else {
		cmnd << outputName << " = " << "(" << nom << " * " << inputName << ")/(" << dem << ")";
		nle.processCommands(cmnd.toString());
	}



}

