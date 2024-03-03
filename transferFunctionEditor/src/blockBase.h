#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>
#include <gui/View.h>
#include "../../EquationToXML/inc/nodes.h"
#include <utility>
#include "gui/CheckBox.h"
#include "gui/Button.h"
#include "gui/Label.h"
#include "gui/VerticalLayout.h"

#define FONT_ID gui::Font::ID::SystemRegular



class BlockBase {
public:

	class settingsView : public gui::View {
		BlockBase* currentBlock = nullptr;
		gui::CheckBox checkBoxSwitch;
		gui::Button buttonDisconnect;
	public:
		settingsView() :
			checkBoxSwitch("switch input/output"),
			buttonDisconnect("disconnect wires", "remove all incomming and outgoing connections from the selected block")
		{

		}

		bool onClick(gui::CheckBox* pBtn) {
			currentBlock->switchInput();
			return true;
		}

		bool onClick(gui::Button* pBtn) {
			currentBlock->removeConnections();
			return false;
		}

		friend class BlockBase;
	};



protected:

	gui::Rect _r;
	bool switchOutput = false;
	std::vector<std::pair<BlockBase*, int>> connectedTo;
	std::vector<std::pair<BlockBase*, int>> connectedFrom;

	bool disableSetUp = false;


public:
	BlockBase(const gui::Point& position);
	const gui::Rect& getRect() const;
	const bool getInputSwitched() const;
	virtual const gui::Point& getOutput(int poz) const = 0;
	virtual const gui::Point& getInput(int poz) const = 0;
	virtual int getOutputCnt() const = 0;
	virtual int getInputCnt() const = 0;
	const gui::Point getLocation() const;
	const std::vector<std::pair<BlockBase*, int>>& getConnectedToBlocks() const;
	const std::vector<std::pair<BlockBase*, int>>& getConnectedFromBlocks() const;


	virtual void setUpAll() = 0;
	virtual void setUpWires(bool refreshCanvas) = 0;
	void switchInput();
	void setPosition(const gui::Point& position);
	void removeConnections();
	void disconnectInput(int poz);
	void disconnectOutput(int poz);

	bool intersectsBlock(const gui::Point&);
	int intersectsInput(const gui::Point&) const;
	int intersectsOutput(const gui::Point&) const;


	void connectTo(BlockBase* block, int pozFrom, int pozTo);

	virtual void drawBlock(td::ColorID color) = 0;
	void disableLogic(bool disable);


	virtual gui::View& updateSettingsView(settingsView& view);

	//virtual void writeToModel(modelNode& model) = 0;
	//virtual void saveToFile

	virtual ~BlockBase();

};


