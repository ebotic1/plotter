#include "canvas.h"
#include "tBlock.h"
#include "sumBlock.h"
#include "nlBlock.h"
#include "blockInterface.h"
#include <gui/Application.h>


#define BLOCK_COLOR td::ColorID::SysText
#define BLOCK_COLOR_SELECTED td::ColorID::DeepSkyBlue



inline bool kanvas::getModelSize(gui::Size& size) const {
	size.height = 0;
	size.width = 0;
	int best = -1;
	for (int i = 0; i < blocks.size(); ++i) {
		if (blocks[i]->getRect().right > size.width)
			size.width = blocks[i]->getRect().right;
		if (blocks[i]->getRect().bottom > size.height)
			size.height = blocks[i]->getRect().bottom;

	}


	size.height += 1500;
	size.width += 1500;

	return true;
}

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::PrimaryDblClick, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove }) {




}


void kanvas::onDraw(const gui::Rect& rect) {

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i] != tempCurrentBlock.TFBlock)
			blocks[i]->drawBlock(BLOCK_COLOR);
		else
			blocks[i]->drawBlock(BLOCK_COLOR_SELECTED);



	if (lastAction == Actions::wiring) {
		gui::Shape::drawLine(tempCurrentBlock.TFBlock->getOutput(tempCurrentBlock.outputPoz), lastMousePos, BLOCK_COLOR, 2, td::LinePattern::Dash);
	}


}


inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
	this->setFocus(false);

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsBlock(inputDevice.getModelPoint())) {
			lastMousePos = inputDevice.getModelPoint();
			lastAction = Actions::dragging;
			if (blocks[i] == tempCurrentBlock.TFBlock)
				return;
			tempCurrentBlock.TFBlock = blocks[i];
			globals::switcher->showSettings(tempCurrentBlock.TFBlock);
			reDraw();
			return;
		}

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsOutput(inputDevice.getModelPoint()) != -1) {
			tempCurrentBlock.TFBlock = blocks[i];
			tempCurrentBlock.outputPoz = blocks[i]->intersectsOutput(inputDevice.getModelPoint());
			lastAction = Actions::wiring;
			return;
		}

	tempCurrentBlock.TFBlock = nullptr;
	globals::switcher->showSettings(nullptr);
	reDraw();

}

inline void kanvas::onCursorDragged(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		lastMousePos = inputDevice.getModelPoint();
		reDraw();
		return;
	}
	if (lastAction == Actions::dragging) {
		gui::CoordType deltaX = inputDevice.getModelPoint().x - lastMousePos.x;
		gui::CoordType deltaY = inputDevice.getModelPoint().y - lastMousePos.y;

		tempCurrentBlock.TFBlock->setPosition({ tempCurrentBlock.TFBlock->getRect().left + deltaX, tempCurrentBlock.TFBlock->getRect().top + deltaY });
		lastMousePos = inputDevice.getModelPoint();
		return;
	}

}

inline void kanvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		for (int i = 0; i < blocks.size(); ++i)
			if (blocks[i]->intersectsInput(inputDevice.getModelPoint()) != -1) {
				const int& poz = blocks[i]->intersectsInput(inputDevice.getModelPoint());
				tempCurrentBlock.TFBlock->connectTo(blocks[i], tempCurrentBlock.outputPoz, poz);
				break;
			}
		lastAction = Actions::none;
		reDraw();
		return;
	}

	lastAction = Actions::none;
}

inline void kanvas::onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) {
	//createBlock(inputDevice.getModelPoint());
}


inline bool kanvas::onZoom(const gui::InputDevice& inputDevice) {
	scale *= inputDevice.getScale() > 1 ? 1.5 : 0.66667;
	this->scaleToPoint(inputDevice.getScale() , inputDevice.getModelPoint());
	return true;
}



inline bool kanvas::saveState(const td::String& file){

	arch::FileSerializerOut temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveOut out("TFv2", temp);
	try {
		out << int(blocks.size());
		for (BlockBase* b : blocks)
			b->saveToFile(out);

		for (int i = 0; i < blocks.size(); ++i) {
			auto& vec = blocks[i]->getConnectedToBlocks();
			out << int(vec.size());

			for  (auto &set : vec) {
				out << int(set.size());
				for (auto& par : set) {
					auto it = std::find(blocks.begin(), blocks.end(), par.first);
					out << int(it - blocks.begin()) << par.second;
				}
			}
		}

		out << globals::model_settings->name.getValue().strVal();
		td::Variant v;
		globals::model_settings->edit.getValue(v);

		out << v.strVal();

		blockInterface::writeState(out);

	}
	catch (...){
		return false;
	}

	return true;

}

inline bool kanvas::restoreState(const td::String& file){
	
	arch::FileSerializerIn temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveIn in(temp);
	in.setSupportedMajorVersion("TFv2");
	std::vector<BlockBase*> kopija;
	td::Variant titl_v, param_v;
	blockInterface::saveState();

	try {
		int size = 0;
		in >> size;
		kopija.resize(size, nullptr);

		td::String ID;
		for (int i = 0; i < size; ++i) {
			in >> ID;
			if (ID == TFBlock::getID())
				kopija[i] = TFBlock::restoreFromFile(in);
			else if (ID == sumBlock::getID())
				kopija[i] = sumBlock::restoreFromFile(in);
			else if (ID == NLBlock::getID())
				kopija[i] = NLBlock::restoreFromFile(in);
			else
				throw std::logic_error("unknown block");

			kopija[i]->disableLogic(false);
			kopija[i]->setUpBlock();
		}
		for (int i = 0; i < size; ++i) {
			int setCnt, connectionsCnt;
			in >> connectionsCnt;
			for (int j = 0; j < connectionsCnt; ++j) {
				in >> setCnt;
				for (int k = 0; k < setCnt; ++k) {
					int broj, ulazCnt;
					in >> broj >> ulazCnt;
					kopija[i]->connectTo(kopija[broj], j, ulazCnt);
				}
			}

		}


		for (auto& block : kopija)
			block->setUpWires(true);
		

		td::String titl;
		in >> titl;
		titl_v = std::move(titl);

		td::String params;
		in >> params;
		param_v = std::move(params);
		
		blockInterface::reloadState(in);

	}
	catch (...) {
		blockInterface::restoreState();
		for (int i = 0; i < kopija.size(); ++i)
			delete kopija[i];
		return false;
	}


	resetModel(false);


	blocks = std::move(kopija);
	globals::model_settings->edit.setValue(param_v, false);
	globals::model_settings->name.setValue(titl_v);

	currentPath = file;

	return true;
}


inline bool kanvas::onKeyPressed(const gui::Key& key)
{
	if (key.getVirtual() == gui::Key::Virtual::Delete) {
		if (tempCurrentBlock.TFBlock != nullptr) {
			tempCurrentBlock.TFBlock->removeConnections();
			blocks.erase(std::find(blocks.begin(), blocks.end(), tempCurrentBlock.TFBlock));
			globals::switcher->showSettings(nullptr);
			tempCurrentBlock.TFBlock = nullptr;
			lastAction = Actions::none;
			reDraw();
		}
		return true;
	}

	return false;
}


void kanvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice) {
	lastAction = Actions::secondary;
	lastMousePos = inputDevice.getModelPoint();	
}


void kanvas::onSecondaryButtonReleased(const gui::InputDevice& inputDevice)
{
	if (lastAction == Actions::secondary) {
		Frame::openContextMenu(100, inputDevice);
	}
	lastAction = Actions::none;
}

void kanvas::onCursorMoved(const gui::InputDevice& inputDevice)
{
	/*
	if (lastAction == Actions::translate) {
		gui::CoordType deltaX = inputDevice.getModelPoint().x - lastMousePos.x;
		gui::CoordType deltaY = inputDevice.getModelPoint().y - lastMousePos.y;

		trans.translate({ deltaX, deltaY });
		reDraw();

		lastMousePos = inputDevice.getModelPoint();
		return;
	}

	if (lastAction == Actions::secondary)
		lastAction = Actions::translate;

	*/
}

void kanvas::resetModel(bool resetCnt)
{
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
	blocks.clear();
	globals::switcher->showView(0);
	if (resetCnt) {
		TFBlock::resetCnt();
		sumBlock::resetCnt();
		NLBlock::resetCnt();
	}
	globals::model_settings->edit.clean();
}

inline bool kanvas::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu)
{

	return false;
}

inline bool kanvas::onActionItem(gui::ActionItemDescriptor& aiDesc) {

	if (aiDesc._menuID == 100) {
		if (aiDesc._actionItemID == 10) {
			blocks.push_back(new TFBlock(lastMousePos));
			return true;
		}
		if (aiDesc._actionItemID == 11) {
			blocks.push_back(new sumBlock(lastMousePos, true));
			return true;
		}
		if (aiDesc._actionItemID == 12) {
			blocks.push_back(new NLBlock(lastMousePos));
			return true;
		}
	}


	if (aiDesc._menuID == 1) {
		if (aiDesc._actionItemID == 1) {
			resetModel();
		}
	}
	

	if (aiDesc._actionItemID == 2) { // open
		gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Open model", { {"TFeditor save file", "*.tfstate"} }, "Open");
		p->openModalWithID(2,this);
		return true;
	}


	if (aiDesc._actionItemID == 3) { // save

		if (currentPath.isNull()) {
			showAlert("error", "No file is currently open (use 'save as')");
			return true;
		}

		if (!saveState(currentPath))
			showAlert("error", "cannot save file");
		return true;

	}

	if (aiDesc._actionItemID == 4) { // save as
		gui::SaveFileDialog* p = new gui::SaveFileDialog(this, "Save as", { {"TFeditor save file", "*.tfstate"} }, "SavedModel.tfstate");
		p->openModalWithID(4, this);
		return true;
	}



	if (aiDesc._actionItemID == 100) { // export
		gui::SaveFileDialog* p = new gui::SaveFileDialog(this, "Export to", ".xml", "Export");
		p->openModalWithID(5, this);
		return true;
	}
	return false;
}



inline bool kanvas::onClick(gui::FileDialog* pDlg, td::UINT4 dlgID) {
	
	if (dlgID == 2) {//open
		if (!restoreState(pDlg->getFileName()))
			showAlert("error", "cannot open file");
		return true;
	}

	if (dlgID == 4) {//save as
		if (!saveState(pDlg->getFileName()))
			showAlert("error", "cannot open file");
		else
			currentPath = pDlg->getFileName();
		return true;
	}

	if (dlgID == 5) {//export
		if (!exportToXML(pDlg->getFileName()))
			showAlert("error", "cannot export model");
		return true;
	}
	
	return false;
}



bool kanvas::exportToXML(const td::String &path){

	xml::Writer w;
	w.open(path);
	w.startDocument();
	if (!w.isOk())
		return false;

	modelNode mod;

	mod.processCommands("Params:\nVars:");
	BlockBase::Nodes nodes;

	baseNode& params = *mod.getNodes()[0];


	for (auto &block : blocks)
		block->writeToModel(mod, nodes);

	td::Variant v;
	globals::model_settings->edit.getValue(v);

	params.processCommands("t // time");
	params.processCommands(v.strVal());

	mod.attribs = std::move(params.attribs);
	params.attribs.clear();

	mod["domain"] = "real";
	mod["type"] = "DAE";
	mod["name"] = globals::model_settings->name.getValue().strVal();

	mod.printNode(w);

	w.endDocument();
	w.close();

	return true;
}


inline kanvas::~kanvas() {
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}