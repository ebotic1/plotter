#pragma once
#include <gui/Canvas.h>
#include <vector>
#include <arch/ArchiveIn.h>
#include <arch/ArchiveOut.h>
#include <arch/FileSerializer.h>
#include <gui/FileDialog.h>
#include "./../../../EquationToXML/inc/nodes.h"
#include "gui/Transformation.h"
#include <gui/Transformation.h>
#include "propertySwitcher.h"



class kanvas : public gui::Canvas {
	std::vector<BlockBase *> blocks;
	gui::Point lastMousePos = { 0,0 };
	double scale = 1;
	gui::Point scroll = { 0,0 };
	gui::Transformation transformation;

	enum class Actions{none, wiring, dragging, secondary, translate} lastAction = Actions::none;
	struct blockInfo {
		BlockBase* TFBlock = nullptr;
		int outputPoz;

	}tempCurrentBlock;


	td::String currentPath;

	properties* props;

public:
	kanvas(properties *props);
	properties* getProperties();
	kanvas* getCanvas();

	void onDraw(const gui::Rect& rect) override;
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice);
	void onCursorDragged(const gui::InputDevice& inputDevice);
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice);
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice);
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice);
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice);
	void onCursorMoved(const gui::InputDevice& inputDevice);
	bool onZoom(const gui::InputDevice& inputDevice) override;
	bool onScroll(const gui::InputDevice& inputDevice);

	gui::Point getModelPoint(const gui::Point &framePoint);
	gui::Point getFramePoint(const gui::Point& framePoint);

	bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
	bool onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) override;

	void getModel(modelNode& model);
	bool saveState(const td::String &file, const td::String &settingsString);
	bool restoreState(const td::String &file, td::String &settingsString);
	bool onKeyPressed(const gui::Key& key);

	bool exportToXML(const td::String& path);
	void resetModel(bool resetCnt = true);
	void reDraw();

	virtual ~kanvas();

	
};


