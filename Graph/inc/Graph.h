#pragma once
#include "./../../common/GraphLib.h"
#include "../inc/GraphDrawer.h"
#include <gui/Canvas.h>
#include <gui/Transformation.h>

class GRAPH_API Graph: public gui::Canvas, public GraphDrawer
{
	static const std::initializer_list<gui::InputDevice::Event> inputs;
	static const std::initializer_list<gui::InputDevice::Event> noInputs;

	enum class Actions { none, select, secondaryClick, drag, pointPeek, moveLegend } action = Actions::none;	
	gui::Point _lastMousePos;

	td::ColorID _backgroundColor;

	struct imageButton {
		gui::Image image;
		gui::Rect rect;

		imageButton(const gui::Image& image, const gui::Rect& rect) : image(image), rect(rect) {};
	};


	std::vector<imageButton> _slike;
	bool _drawButtons = true;
	gui::Transformation _tr;
	gui::Point _trPoint;
	gui::Size _realSize;

	gui::Rect _selectRect;
	td::String _txtPut, _lastPath;

protected:

	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonPressed(const gui::InputDevice& inputDevice) override;
	void onSecondaryButtonReleased(const gui::InputDevice& inputDevice) override;
	void onCursorMoved(const gui::InputDevice& inputDevice) override;
	void onCursorDragged(const gui::InputDevice& inputDevice) override;
	bool onZoom(const gui::InputDevice& inputDevice) override;
	bool onKeyPressed(const gui::Key& key) override;
	void onCursorExited(const gui::InputDevice& inputDevice) override;
	void onCursorEntered(const gui::InputDevice& inputDevice) override;
	void onPrimaryButtonDblClick(const gui::InputDevice& inputDevice) override;
	bool onScroll(const gui::InputDevice& inputDevice) override;

	void onDraw(const gui::Rect& rect) override;
	void onResize(const gui::Size& newSize) override;

public:

	Graph(gui::Font *fontAxis, gui::Font *fontLegend, td::ColorID backgroundColor = td::ColorID::SysCtrlBack, bool drawButtons = true, bool takeUserInput = true);

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
	
	void setBackgroundColor(td::ColorID color);
	td::ColorID getBackgroundColor() { return _backgroundColor; }
	void drawButtons(bool draw);	

	void showInformation();
	void showHelp();
	void saveMenu();
	void readTXT(const td::String& path);
	void readXML(const td::String& path, bool onlyData);

	void buttonAction(int buttonIndex);
	static inline const char * buttonsImages[] = \
	{":fullScreen", ":grid", ":legend", ":meni", ":save", ":reset", ":fitToWindow", ":info", ":graph_settings"};
	
	bool saveXML(const td::String& path);
	void saveTXT(const td::String& path);
	bool saveTXT(const td::String& path, bool horizontal);
	bool openFile(td::String path, bool readOnlyFunctions = false);
	bool save(const td::String& path);
	
	void drawAgain() override{
		reDraw();
	};
	~Graph(){

	}
};