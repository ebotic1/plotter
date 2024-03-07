#pragma once
#include "gui/Shape.h"
#include "gui/DrawableString.h"
#include <vector>
#include "./../src/blockBase.h"
#include "squareBlock.h"

#define FONT_ID gui::Font::ID::SystemNormal

class TFBlock: public squareBlockSI, public squareBlockSO, public squareBlock{

public:
	class settingsView :
		public squareBlockSI::settingsView,
		public squareBlockSO::settingsView,
		public BlockBase::settingsView
	{

		elementProperty num, dem;
		gui::VerticalLayout vL;
		TFBlock* currentBlock = nullptr;
	public:
		settingsView():
			vL(6),
			num("numerator: ", td::string8, "Numerator of the transfer function (use 's' as a variable)"),
			dem("denominator: ", td::string8, "Denominator of the transfer function (use 's' as a variable)")
		{
			dem.Action = [this](const td::Variant& v) {currentBlock->setDenominator(v.strVal()); };
			num.Action = [this](const td::Variant& v) {currentBlock->setNumerator(v.strVal()); };
			vL << num << dem << BlockBase::settingsView::vL << squareBlockSI::settingsView::vL << squareBlockSO::settingsView::vL;
			vL.appendSpacer();
			setLayout(&vL);
		}
		friend class TFBlock;
	};
private:

	gui::DrawableString drawNom, drawDem;
	gui::Rect rectangleNominator, rectangleDenominator;
	gui::Shape fracLine;

	td::String nom, dem;


	gui::DrawableString drawUlaz, drawIzlaz;
	gui::Rect inputRect, outputRect;

	bool hasLaplaceOperator(const td::String& s);
	static int cnt; //za brojanje koliko blokova je kreirano

	TFBlock();
public:
	static void resetCnt() { cnt = 0; }
	TFBlock(const gui::Point &position, const td::String &inputName, const td::String& outputName);
	TFBlock(const gui::Point& position);

	void setUpBlock();
	void setUpAll(bool ignoreRelatedBlocks = false) override;

	void setNumerator(const td::String& nominator);
	void setDenominator(const td::String& denominator);

	gui::View& getSettingView() {
		return *new gui::View();
	}
	void drawBlock(td::ColorID color);
	void updateSettingsView(BlockBase::settingsView* view);
	void writeToModel(modelNode& model, Nodes& nodes) override;

	~TFBlock();

	static td::String getID();


	void saveToFile(arch::ArchiveOut& f) override;
	static TFBlock *restoreFromFile(arch::ArchiveIn& f);

};

