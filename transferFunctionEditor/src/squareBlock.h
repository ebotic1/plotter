#pragma once
#include "blockBase.h"
#include <vector>
#include "gui/Shape.h"
#include "td/String.h"

class squareBlock : virtual public BlockBase {
protected:
	std::vector<gui::Shape> connectionLines;

public:
	squareBlock(){}
	void setUpWires(bool refreshCanvas);

};


class squareBlockSI : virtual public BlockBase {
protected:
	td::String ulazName;
	gui::Point inputPoint;
public:
	squareBlockSI(const td::String& inputName): ulazName(inputName){
		connectedFrom.resize(1);
	}
	const gui::Point& getInput(int poz) const {
		return inputPoint;
	}
	int getInputCnt() const { return 1; }

	void setInputName(const td::String& name);

};


class squareBlockSO : virtual public BlockBase {
public:
	class settingsView : public gui::View {
		gui::VerticalLayout vL;
		elementProperty outputProp;
		
	public:
		settingsView() :
			vL(1),
			outputProp("output var: ", td::string8, "variable name that will be created when exporting XML file for this block's output")
		{
			
		}
		friend class squareBlockSO;
	};
protected:
	td::String izlazName;
	gui::Point outputPoint;
public:
	squareBlockSO(const td::String& outputName): izlazName(outputName){
		connectedTo.resize(1);
	}

	const gui::Point& getOutput(int poz) const {
		return outputPoint;
	}
	int getOutputCnt() const { return 1; }

	void setOutputName(const td::String& name);
	virtual gui::View& updateSettingsView(settingsView* view);
};
