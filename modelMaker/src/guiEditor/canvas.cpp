#include "canvas.h"
#include "tBlock.h"
#include "sumBlock.h"
#include "nlBlock.h"
#include "blockInterface.h"
#include <gui/Application.h>


#define BLOCK_COLOR td::ColorID::SysText
#define BLOCK_COLOR_SELECTED td::ColorID::DeepSkyBlue


kanvas::kanvas(properties* props): gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, \
	gui::InputDevice::Event::PrimaryDblClick, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, \
	gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove }),
	props(props)
{
	this->registerForScrollEvents();

}

properties* kanvas::getProperties()
{
	return props;
}

kanvas* kanvas::getCanvas()
{
	return this;
}


void kanvas::onDraw(const gui::Rect& rect) {

	if (lastAction == Actions::wiring) {
		gui::Shape::drawLine(getModelPoint(tempCurrentBlock.TFBlock->getOutput(tempCurrentBlock.outputPoz)), getFramePoint(lastMousePos), BLOCK_COLOR, 2, td::LinePattern::Dash);
	}

	transformation.appendToContext();

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i] != tempCurrentBlock.TFBlock)
			blocks[i]->drawBlock(BLOCK_COLOR);
		else
			blocks[i]->drawBlock(BLOCK_COLOR_SELECTED);






}


inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
	this->setFocus(false);

	auto point = getFramePoint(inputDevice.getFramePoint());

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsBlock(point)) {
			lastMousePos = point;
			lastAction = Actions::dragging;
			if (blocks[i] == tempCurrentBlock.TFBlock)
				return;
			tempCurrentBlock.TFBlock = blocks[i];
			props->showSettings(tempCurrentBlock.TFBlock);
			reDraw();
			return;
		}

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsOutput(point) != -1) {
			tempCurrentBlock.TFBlock = blocks[i];
			tempCurrentBlock.outputPoz = blocks[i]->intersectsOutput(point);
			lastAction = Actions::wiring;
			return;
		}

	tempCurrentBlock.TFBlock = nullptr;
	props->showSettings(nullptr);
	reDraw();

}

inline void kanvas::onCursorDragged(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		lastMousePos = getModelPoint(inputDevice.getFramePoint());
		reDraw();
		return;
	}
	if (lastAction == Actions::dragging) {
		gui::CoordType deltaX = getModelPoint(inputDevice.getFramePoint()).x - lastMousePos.x;
		gui::CoordType deltaY = getModelPoint(inputDevice.getFramePoint()).y - lastMousePos.y;

		tempCurrentBlock.TFBlock->setPosition({ tempCurrentBlock.TFBlock->getRect().left + deltaX, tempCurrentBlock.TFBlock->getRect().top + deltaY });
		lastMousePos = getModelPoint(inputDevice.getFramePoint());
		return;
	}

}

inline void kanvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		for (int i = 0; i < blocks.size(); ++i)
			if (blocks[i]->intersectsInput(inputDevice.getFramePoint()) != -1) {
				const int& poz = blocks[i]->intersectsInput(inputDevice.getFramePoint());
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
	auto s = inputDevice.getScale() > 1 ? 1.5 : 0.66667;
	scale *= s;
	transformation.scale(s);
	scroll.x /= s;
	scroll.y /= s;
	reDraw();
	return true;
}

bool kanvas::onScroll(const gui::InputDevice& inputDevice)
{
	
	auto delta = inputDevice.getScrollDelta();
	bool shift = inputDevice.getKey().isShiftPressed();
	delta.y = delta.y * 10;
	delta.x = -delta.y * shift;
	delta.y *= -1 * !shift;
	transformation.translate(delta);
	scroll += delta;
	reDraw();
	return true;
}

gui::Point kanvas::getModelPoint(const gui::Point& framePoint)
{
	return { (framePoint.x + scroll.x) * scale, (framePoint.y + scroll.y) * scale };
}

gui::Point kanvas::getFramePoint(const gui::Point& framePoint)
{
	return { framePoint.x / scale - scroll.x, (framePoint.y) / scale - scroll.y };
}




void kanvas::getModel(modelNode& mod)
{
	mod.clear();

	mod.processCommands("Params:\nVars:");
	BlockBase::Nodes nodes;

	baseNode& params = *mod.getNodes()[0];


	for (auto& block : blocks)
		block->writeToModel(mod, nodes);

	td::Variant v;
	props->getModelSettings()->edit.getValue(v);

	params.processCommands("t // time");
	params.processCommands(v.strVal());

	mod.attribs = std::move(params.attribs);
	params.attribs.clear();

	mod["domain"] = "real";
	mod["type"] = "DAE";
	mod["name"] = props->getModelSettings()->name.getValue().strVal();

}

bool kanvas::saveState(const td::String& file, const td::String& settingsString)
{
	arch::FileSerializerOut temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveOut out("TFv3", temp);
	try {
		out << settingsString;
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

		out << props->getModelSettings()->name.getValue().strVal();
		td::Variant v;
		props->getModelSettings()->edit.getValue(v);

		out << v.strVal();

		blockInterface::writeState(out);

	}
	catch (...){
		return false;
	}

	return true;

}

bool kanvas::restoreState(const td::String& file, td::String& settingsString)
{	
	arch::FileSerializerIn temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveIn in(temp);
	in.setSupportedMajorVersion("TFv3");
	std::vector<BlockBase*> kopija;
	td::Variant titl_v, param_v;
	blockInterface::saveState();

	try {
		settingsString.clean();
		in >> settingsString;
		int size = 0;
		in >> size;
		kopija.resize(size, nullptr);

		td::String ID;
		for (int i = 0; i < size; ++i) {
			in >> ID;
			if (ID == TFBlock::getID())
				kopija[i] = TFBlock::restoreFromFile(in, this);
			else if (ID == sumBlock::getID())
				kopija[i] = sumBlock::restoreFromFile(in, this);
			else if (ID == NLBlock::getID())
				kopija[i] = NLBlock::restoreFromFile(in, this);
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
	props->getModelSettings()->edit.setValue(param_v, false);
	props->getModelSettings()->name.setValue(titl_v);

	currentPath = file;

	return true;
}


inline bool kanvas::onKeyPressed(const gui::Key& key)
{
	if (key.getVirtual() == gui::Key::Virtual::Delete) {
		if (tempCurrentBlock.TFBlock != nullptr) {
			tempCurrentBlock.TFBlock->removeConnections();
			blocks.erase(std::find(blocks.begin(), blocks.end(), tempCurrentBlock.TFBlock));
			props->showSettings(nullptr);
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
	lastMousePos = getModelPoint(inputDevice.getModelPoint());	
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
	props->showView(0);
	if (resetCnt) {
		TFBlock::resetCnt();
		sumBlock::resetCnt();
		NLBlock::resetCnt();
	}
	props->getModelSettings()->edit.clean();
}

void kanvas::reDraw()
{
	gui::Frame::reDraw();
}

inline bool kanvas::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu)
{

	return false;
}

inline bool kanvas::onActionItem(gui::ActionItemDescriptor& aiDesc) {

	if (aiDesc._menuID == 100) {
		if (aiDesc._actionItemID == 10) {
			blocks.push_back(new TFBlock(lastMousePos, this));
			return true;
		}
		if (aiDesc._actionItemID == 11) {
			blocks.push_back(new sumBlock(lastMousePos, this, true));
			return true;
		}
		if (aiDesc._actionItemID == 12) {
			blocks.push_back(new NLBlock(lastMousePos, this));
			return true;
		}
	}

	return false;
}





bool kanvas::exportToXML(const td::String &path){

	modelNode mod;
	getModel(mod);
	return mod.printNode(path);

}


inline kanvas::~kanvas() {
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}