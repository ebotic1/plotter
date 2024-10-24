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
#include "blockBase.h"
#include "../BaseView.h"

class properties;

class kanvas : public gui::Canvas {
	std::vector<BlockBase *> blocks;
	gui::Point lastMousePos = { 0,0 };
	double scale = 1;
	gui::Point totalScroll = { 0,0 };
	gui::Transformation transformation;

	enum class Actions{none, wiring, dragging, secondary, translate} lastAction = Actions::none;
	struct blockInfo {
		BlockBase* TFBlock = nullptr;
		int outputPoz;

	}tempCurrentBlock;


	td::String currentPath;

	properties* props;
	ViewForTab::BaseClass* tabCore;

public:
	kanvas(properties *props, ViewForTab::BaseClass * tabCore);
	properties* getProperties();
	kanvas* getCanvas();

	void onDraw(const gui::Rect& rect) override;
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onCursorDragged(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onCursorMoved(const gui::InputDevice& inputDevice) override;
	void zoom(double factor);
	bool onZoom(const gui::InputDevice& inputDevice) override;
	void scroll(gui::Point delta);
	bool onScroll(const gui::InputDevice& inputDevice) override;

	gui::Point getModelPoint(const gui::Point &framePoint);
	gui::Point getFramePoint(const gui::Point& framePoint);

	bool onActionItem(gui::ActionItemDescriptor& aiDesc) override;
	bool onContextMenuUpdate(td::BYTE menuID, gui::ContextMenu* pMenu) override;

	void getModel(modelNode& model);
	bool saveState(const td::String &file, const td::String &settingsString);
	bool restoreState(const td::String &file, td::String &settingsString);
	bool onKeyPressed(const gui::Key& key) override;

	bool exportToXML(const td::String& path);
	void resetModel(bool resetCnt = true);
	void reDraw();

	virtual ~kanvas();

	
};


