#include "canvas.h"

#define BLOCK_COLOR td::ColorID::Black
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

kanvas::kanvas() : gui::Canvas({ gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::SecondaryClicks, gui::InputDevice::Event::PrimaryDblClick, gui::InputDevice::Event::Zoom, gui::InputDevice::Event::CursorDrag, gui::InputDevice::Event::Keyboard }) {




}


void kanvas::onDraw(const gui::Rect& rect) {
	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i] != tempCurrentBlock.Block)
			blocks[i]->drawBlock(BLOCK_COLOR);
		else
			blocks[i]->drawBlock(BLOCK_COLOR_SELECTED);



	if (lastAction == Actions::wiring) {
		gui::Shape::drawLine(tempCurrentBlock.Block->getOutput(tempCurrentBlock.outputPoz), lastMousePos, BLOCK_COLOR, 2, td::LinePattern::Dash);
	}


}

inline void kanvas::onPrimaryButtonPressed(const gui::InputDevice& inputDevice) {
	this->setFocus(false);

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsBlock(inputDevice.getModelPoint())) {
			lastMousePos = inputDevice.getModelPoint();
			lastAction = Actions::dragging;
			if (blocks[i] == tempCurrentBlock.Block)
				return;
			tempCurrentBlock.Block = blocks[i];
			globals::switcher->showSettings(typeid(*tempCurrentBlock.Block).hash_code());
			reDraw();
			return;
		}

	for (int i = 0; i < blocks.size(); ++i)
		if (blocks[i]->intersectsOutput(inputDevice.getModelPoint()) != -1) {
			tempCurrentBlock.Block = blocks[i];
			tempCurrentBlock.outputPoz = blocks[i]->intersectsOutput(inputDevice.getModelPoint());
			lastAction = Actions::wiring;
			return;
		}

	tempCurrentBlock.Block = nullptr;
	globals::switcher->showSettings(0);
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

		tempCurrentBlock.Block->setPosition({ tempCurrentBlock.Block->getRect().left + deltaX, tempCurrentBlock.Block->getRect().top + deltaY });
		lastMousePos = inputDevice.getModelPoint();
		return;
	}

}

inline void kanvas::onPrimaryButtonReleased(const gui::InputDevice& inputDevice) {
	if (lastAction == Actions::wiring) {
		for (int i = 0; i < blocks.size(); ++i)
			if (blocks[i]->intersectsInput(inputDevice.getModelPoint()) != -1) {
				const int& poz = blocks[i]->intersectsInput(inputDevice.getModelPoint());
				tempCurrentBlock.Block->connectTo(blocks[i], poz, tempCurrentBlock.outputPoz);
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
	this->scaleToPoint(scale, inputDevice.getModelPoint());
	return true;
}


/*
inline bool kanvas::saveState(const td::String& file){

	arch::FileSerializerOut temp;
	if (!temp.open(file))
		return false;

	arch::ArchiveOut out("TFv1", temp);
	try {
		out << int(blocks.size());
		for(Block * var : blocks) {
			td::String nom, dem, inputName, outputName;
			bool connected, switched;
			var->getAllProps(nom, dem, connected, switched, inputName, outputName);
			double x, y;
			x = var->getLocation().x;
			y = var->getLocation().y;
			out << x << y;
			out << nom << dem << switched << inputName << outputName;
		}

		for (int i = 0; i < blocks.size(); ++i) {
			auto& vec = blocks[i]->getConnectedBlocks();
			out << int(vec.size());

			for  (Block * var : vec) {
				auto it = std::find(blocks.begin(), blocks.end(), var);
				out << int(it - blocks.begin());
			}
		}
		out << cntBlock;
		out << globals::model_settings->name.getValue().strVal();
		td::Variant v;
		globals::model_settings->edit.getValue(v);

		out << v.strVal();


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
	in.setSupportedMajorVersion("TFv1");
	std::vector<Block*> kopija;


	try {
		int size = 0;
		in >> size;
		kopija.resize(size, nullptr);
		for (int i = 0; i < size; ++i) {
			td::String nom, dem, inputName, outputName;
			bool connected, switched;
			double x, y;

			in >> x >> y;
			in >> nom >> dem >> switched >> inputName >> outputName;

			kopija[i] = new Block({ x,y }, inputName, outputName);
			kopija[i]->disableLogic(true);
			if(switched)
				kopija[i]->switchInput();
			kopija[i]->setNominator(nom);
			kopija[i]->setDenominator(dem);
		}
		for (int i = 0; i < size; ++i) {
			int connectionsCnt;
			in >> connectionsCnt;
			for (int j = 0; j < connectionsCnt; ++j) {
				int broj;
				in >> broj;
				kopija[i]->connectTo(kopija[broj]);
			}
			kopija[i]->disableLogic(false);
			kopija[i]->setUpAll();
		}

		in >> cntBlock;

		td::String titl;
		in >> titl;
		globals::model_settings->name.setValue(titl);

		td::String params;
		in >> params;
		td::Variant v(std::move(params));
		globals::model_settings->edit.setValue(v, false);

	}
	catch (...) {
		for (int i = 0; i < kopija.size(); ++i)
			delete kopija[i];
		return false;
	}


	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
	blocks = std::move(kopija);

	currentPath = file;

	return true;
}
*/

inline bool kanvas::onKeyPressed(const gui::Key& key)
{
	if (key.getVirtual() == gui::Key::Virtual::Delete) {
		if (tempCurrentBlock.Block != nullptr) {
			tempCurrentBlock.Block->removeConnections();
			blocks.erase(std::find(blocks.begin(), blocks.end(), tempCurrentBlock.Block));
			tempCurrentBlock.Block = nullptr;
			lastAction = Actions::none;
			reDraw();
		}
		return true;
	}

	return false;
}


void kanvas::onSecondaryButtonPressed(const gui::InputDevice& inputDevice) {
	lastMousePos = inputDevice.getModelPoint();
	Frame::openContextMenu(100, inputDevice);
}


inline bool kanvas::onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu)
{

	return false;
}

inline bool kanvas::onActionItem(gui::ActionItemDescriptor& aiDesc) {

	if (aiDesc._menuID == 100) {
		if (aiDesc._actionItemID == 10) {

			createBlock(lastMousePos);
			return true;

		}
	}

	return false;

	if (aiDesc._menuID == 1) {
		if (aiDesc._actionItemID == 1) {
			for (int i = 0; i < blocks.size(); ++i)
				delete blocks[i];
			blocks.clear();
			globals::switcher->showView(0);
			cntBlock = 0;
			globals::model_settings->edit.clean();
			return true;
		}
	}
	/*

	if (aiDesc._actionItemID == 2) { // open
		gui::OpenFileDialog* p = new gui::OpenFileDialog(this, "Open model", { "*.tfstate" }, "Open");
		p->openModal(2,this);
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
		gui::FileDialog* p = new gui::FileDialog(this, gui::FileDialog::Type::SaveFile, "Save as", ".tfstate", "Save");
		p->openModal(4, this);
		return true;
	}



	if (aiDesc._actionItemID == 100) { // export
		gui::FileDialog* p = new gui::FileDialog(this, gui::FileDialog::Type::SaveFile, "Export to", ".xml", "Export");
		p->openModal(5, this);
		return true;
	}

}

	*/

	return false;
}


inline bool kanvas::onClick(gui::FileDialog* pDlg, td::UINT4 dlgID) {
	/*
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
	*/
	return false;
}


/*
bool kanvas::exportToXML(const td::String &path){

	xml::Writer w;
	w.open(path);
	w.startDocument();
	if (!w.isOk())
		return false;

	modelNode mod;

	mod.processCommands("Vars: ; Params: ; TFs: ; NLEqs:");

	baseNode &vars = *mod.nodes[0];
	baseNode& params = *mod.nodes[1];
	baseNode& tfs = *mod.nodes[2];
	baseNode& nle = *mod.nodes[3];

	for (int i = 0; i < blocks.size(); ++i) {
		td::String nom, dem, inputName, outputName;
		bool connected, switched;

		blocks[i]->getAllProps(nom, dem, connected, switched, inputName, outputName);
		vars.processCommands(inputName);
		if (blocks[i]->getConnectedFromBlocks().empty())
			vars.nodes.back()->attribs["out"] = "true";

		vars.processCommands(outputName);
		if (blocks[i]->getConnectedBlocks().empty())
			vars.nodes.back()->attribs["out"] = "true";


		inputName = inputName.subStr(0, inputName.find("=")-1);
		outputName = outputName.subStr(0, outputName.find("=")-1);
		cnt::StringBuilder cmnd;

		auto hasLaplaceOperator = [](const td::String& s) {
			int poz = 0;

			while (true) {
				poz = s.find("s", poz);
				if (poz == -1)
					break;

				char c = (s.length() > poz + 1) ? s.getAt(poz + 1) : '*';
				if (std::isdigit(c) || c == '*' || c == '/' || c == '+' || c == '-' || c == ' ') {
					c = (poz > 0) ? s.getAt(poz - 1) : c = '*';
					if (c == '*' || c == '/' || c == '+' || c == '-' || c == ' ')
						return true;
				}
				++poz;
			}
			return false;
		};



		if (hasLaplaceOperator(nom) || hasLaplaceOperator(dem)) {
			cmnd << outputName << "/" << inputName << " = " << "(" << nom << ")" << "/(" << dem << ")";
			tfs.processCommands(cmnd.toString());
		}
		else {
			cmnd << outputName << " = " << "(" << nom  << " * " <<  inputName  << ")/(" << dem << ")";
			nle.processCommands(cmnd.toString());
		}


		cmnd.reset();

		if (blocks[i]->getConnectedFromBlocks().size() == 0) {
			inputName = blocks[i]->getInputName();
			if (inputName.find('=') == -1)
				cmnd << inputName << " = 0";
			else
				cmnd << inputName;



			nle.processCommands(cmnd.toString());
			continue;
		}

		cmnd << inputName << " = ";
		auto& connectedFrom = blocks[i]->getConnectedFromBlocks();
		for (int j = 0; j < connectedFrom.size(); ++j){
			cmnd << connectedFrom[j]->getOutputName();
			if (j != connectedFrom.size() - 1)
				cmnd << " + ";
		}

		nle.processCommands(cmnd.toString());

	}

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
*/

inline kanvas::~kanvas() {
	for (int i = 0; i < blocks.size(); ++i)
		delete blocks[i];
}