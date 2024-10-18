#pragma once
#include "squareBlock.h"
#include "blockBase.h"
#include "blockInterface.h"
#include "gui/ComboBox.h"
#include "gui/DrawableString.h"
#include "canvas.h"


class sumBlock : public squareBlockMInameless, public squareBlockSO, public squareBlock 
{
public:
	class settingsView: 
		public squareBlockMInameless::settingsView,
		public squareBlockSO::settingsView,
		public BlockBase::settingsView
	{
		gui::ComboBox izbor;
		sumBlock* currentBlock = nullptr;
	protected:
		gui::VerticalLayout vL;
	public:
		settingsView() :
			vL(6)
		{
			izbor.addItem("-");
			izbor.addItem("+");
			izbor.onChangedSelection([this]() {currentBlock->changeSign(izbor.getSelectedIndex()); });
			vL << izbor << BlockBase::settingsView::vL << squareBlockSO::settingsView::vL << squareBlockMInameless::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class sumBlock;
	};
	friend class settingsView;

private:

	bool sumOperator;
	gui::DrawableString znak;

	static int blockCnt;
	

public:
	sumBlock(gui::Point position, kanvas *parent, bool addition_operator, int cnt = 2);
	virtual int& getCnt() override { return blockCnt; }

	void setUpBlock() override;
	void drawBlock(td::ColorID color) override;
	void writeToModel(modelNode& model, Nodes& nodes) override;
	void saveToFile(arch::ArchiveOut& f) override;
	static sumBlock* restoreFromFile(arch::ArchiveIn& f, kanvas* parent);
	void updateSettingsView(BlockBase::settingsView* view);

	void changeSign(bool addition);
	static void resetCnt() { blockCnt = 0; }

	static td::String getID() { return "sumBlock"; }
	~sumBlock();
	friend class blockInterface;
};