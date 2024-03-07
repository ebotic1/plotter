#pragma once
#include "blockBase.h"
#include <vector>
#include "gui/Shape.h"
#include "td/String.h"

class squareBlock : virtual public BlockBase {

	gui::Rect inputRect, outputRect;

	void createArrow(gui::Shape& arrow, const gui::Point &posBegin, const gui::Point &posEnd, const double &lenght);
	std::vector<gui::Shape> arrows;

	std::vector<gui::Point> inputPoints, outputPoints;

protected:
	std::vector<gui::Shape> connectionLines;
	gui::Shape recShape;

public:
	squareBlock();
	void setUpWires(bool refreshCanvas);
	void drawBlock(td::ColorID color) override;
	void setUpBlock();

	const gui::Point& getInput(int poz) const {
		return inputPoints.at(poz);
	}

	const gui::Point& getOutput(int poz) const {
		return outputPoints.at(poz);
	}
};


class squareBlockSI : virtual public BlockBase {
public:
	class settingsView{
		elementProperty inputProp;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(1),
			inputProp("input var: ", td::string8, "variable name of this block's output")
		{
			vL << inputProp;
		}
		friend class squareBlockSI;
	};
protected:
	td::String ulazName;
	squareBlockSI(){
		connectedFrom.resize(1);
	}
public:
	squareBlockSI(const td::String& inputName): ulazName(inputName){
		connectedFrom.resize(1);
	}

	int getInputCnt() const { return 1; }

	void setInputName(const td::String& name);
	const td::String& getInputName(int pos) const override{ return ulazName; }
	void updateSettingsView(BlockBase::settingsView* view);
};


class squareBlockSO : virtual public BlockBase {
public:
	class settingsView{
		elementProperty outputProp;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(1),
			outputProp("output var: ", td::string8, "variable name of this block's input")
		{
			vL << outputProp;
		}
		friend class squareBlockSO;
	};
protected:
	td::String izlazName;
	squareBlockSO() { connectedTo.resize(1); }

public:
	squareBlockSO(const td::String& outputName);
	int getOutputCnt() const { return 1; }
	const td::String& getOutputName(int pos) const override { return izlazName; }
	void setOutputName(const td::String& name);
	void updateSettingsView(BlockBase::settingsView* view);

};
