#include "squareBlock.h"
#include "blockInterface.h"
#pragma once

class NLBlock : public squareBlock, public squareBlockSO, public squareBlockMI
{
public:
	class settingsView :
		public BlockBase::settingsView,
		public squareBlockSO::settingsView,
		public squareBlockMI::settingsView
	{

		gui::VerticalLayout vL;
		elementProperty equation;
		NLBlock* thisBlock = nullptr;

	public:
		settingsView():
			vL(5),
			equation("Equation:", td::string8, "Nonlinear equation that combines input variables")
		{
			equation.Action = [this](const td::Variant& v) {thisBlock->setEquation(v.strVal()); };
			vL << equation << BlockBase::settingsView::vL << squareBlockSO::settingsView::vL << squareBlockMI::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class NLBlock;
	};

private:

	gui::DrawableString eqDraw;

	static int blockCnt;
	

public:

	NLBlock(const gui::Point &position);
	virtual int& getCnt() override { return blockCnt; }

	void setUpBlock() override;
	void drawBlock(td::ColorID color) override;
	void writeToModel(modelNode& model, Nodes& nodes) override;
	void saveToFile(arch::ArchiveOut& f) override;
	static NLBlock* restoreFromFile(arch::ArchiveIn& f);
	void updateSettingsView(BlockBase::settingsView* view);

	void setEquation(td::String eq);

	static void resetCnt() { blockCnt = 0; }
	static td::String getID() { return "NLBlock"; }
	~NLBlock();
	friend class blockInterface;
};