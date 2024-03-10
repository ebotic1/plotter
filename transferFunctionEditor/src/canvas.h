#pragma once
#include "gui/Canvas.h"
#include <vector>
#include "globals.h"
#include "arch/ArchiveIn.h"
#include "arch/ArchiveOut.h"
#include "arch/FileSerializer.h"
#include "gui/FileDialog.h"
#include "./../../EquationToXML/inc/nodes.h"
#include "gui/Transformation.h"




class kanvas : public gui::Canvas {
	std::vector<BlockBase *> blocks;
	gui::Point lastMousePos = { 0,0 };
	double scale = 1;

	enum class Actions{none, wiring, dragging, secondary, translate} lastAction = Actions::none;
	struct blockInfo {
		BlockBase* TFBlock = nullptr;
		int outputPoz;

	}tempCurrentBlock;


	td::String currentPath;

public:
	kanvas();
	void onDraw(const gui::Rect& rect) override;
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);
	void onCursorDragged(const gui::InputDevice& inputDevice);
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice);
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice);
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice);
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice);
	void onCursorMoved(const gui::InputDevice& inputDevice);
	bool onZoom(const gui::InputDevice& inputDevice) override;
	bool getModelSize(gui::Size& size) const override;

	bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
	bool onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) override;

	virtual bool onClick(gui::FileDialog* pDlg, td::UINT4 dlgID);

	bool saveState(const td::String &file);
	bool restoreState(const td::String &file);
	bool onKeyPressed(const gui::Key& key);

	bool exportToXML(const td::String& path);
	void resetModel(bool resetCnt = true);

	~kanvas();

	friend void globals::refreshCanvas();
	
};


