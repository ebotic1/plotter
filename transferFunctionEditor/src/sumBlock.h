#pragma once
#include "squareBlock.h"
#include "blockBase.h"
#include "gui/ComboBox.h"
#include "gui/DrawableString.h"


class sumBlock : public squareBlockMInameless, public squareBlockSO, public squareBlock 
{
	bool sumOperator;
	gui::DrawableString znak;

	static int cnt; //za brojanje koliko blokova je kreirano


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

public:
	sumBlock(gui::Point position, bool addition_operator, int cnt = 2);

	void setUpBlock() override;
	void drawBlock(td::ColorID color) override;
	void writeToModel(modelNode& model, Nodes& nodes) override;
	void saveToFile(arch::ArchiveOut& f) override;
	static sumBlock* restoreFromFile(arch::ArchiveIn& f);
	void updateSettingsView(BlockBase::settingsView* view);

	void changeSign(bool addition);

	static td::String getID() { return "sumBlock"; }
	~sumBlock();
};