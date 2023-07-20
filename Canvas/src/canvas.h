#include "gui/Canvas.h"
#include "gui/Shape.h"
#include <vector>

#include "./../../common/defineExportImport.h"

struct  kriva {
	gui::CoordType*  x;
	gui::CoordType*  y;
	td::ColorID boja;
	size_t broj;
};

class IMPEXP Canvas : public gui::Canvas {
	
	std::vector<kriva> funkcije;
	std::vector<gui::CoordType> vertikale;
	std::vector<gui::CoordType> horizontale;

	gui::CoordType xLim[2];
	gui::CoordType yLim[2];
	const gui::CoordType* pokToMinY;
	const gui::CoordType* pokToMinX;
	int xLim_index[2];
	int yLim_index[2];

	double initial_height = 1500;
	double initial_width = 1500;

	gui::CoordType shift_x = 0;
	gui::CoordType shift_y = 0;
	double height_scale = 1;
	double width_scale = 1;

	gui::Rect _select;
	bool sel = false;
	bool _drag = false;


	gui::Point mis;
	bool active = false;

	td::ColorID axisColor = td::ColorID::Black;
	gui::CoordType font_height;


	void setScaleFromWindow(gui::Geometry select, gui::Geometry window);
	void updateLimits(const gui::CoordType* x, const gui::CoordType* y, const size_t broj);
	void setLimits(const gui::CoordType* x, const gui::CoordType* y, const size_t broj);
	void drawSetup();
	td::String to_string(gui::CoordType x);


	gui::CoordType roundToNearestPowerOf2(gui::CoordType number);
	void Zoom(double scale);


public:
	Canvas();
	Canvas(const gui::CoordType* x, const gui::CoordType* y, const size_t broj_tacaka);
	Canvas(gui::CoordType* &&x, gui::CoordType* &&y, const size_t broj_tacaka);
	Canvas(const std::vector<gui::CoordType> &x, const std::vector<gui::CoordType> &y, td::ColorID boja = td::ColorID::Black);
	Canvas(std::vector<gui::CoordType>&& x, std::vector<gui::CoordType>&& y, td::ColorID boja = td::ColorID::Black);
	void provjeriVelicinu(const std::vector<gui::CoordType>& x, const std::vector<gui::CoordType>& y);



	~Canvas();

	void Canvas::onDraw(const gui::Rect& rect) override;
	bool Canvas::getModelSize(gui::Size& modelSize) const override;
	virtual bool onZoom(const gui::InputDevice& inputDevice) override;

	virtual void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override;
	virtual void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override;
	virtual void onCursorDragged(const gui::InputDevice& inputDevice) override;
	virtual void onSecondaryButtonPressed(const gui::InputDevice& inputDevice) override;
	virtual void onSecondaryButtonReleased(const gui::InputDevice& inputDevice) override;
	virtual void onCursorMoved(const gui::InputDevice& inputDevice);
	virtual bool onKeyPressed(const gui::Key& key);
	virtual void onCursorExited(const gui::InputDevice& inputDevice);
	virtual void onCursorEntered(const gui::InputDevice& inputDevice);

	void dodajFunkciju(const std::vector<gui::CoordType>& x, const std::vector<gui::CoordType>& y, td::ColorID boja = td::ColorID::Black);
	void dodajFunkciju(std::vector<gui::CoordType>&& x, std::vector<gui::CoordType>&& y, td::ColorID boja = td::ColorID::Black);
	void dodajFunkciju(const gui::CoordType* x, const gui::CoordType* y, const size_t broj_tacaka, td::ColorID = td::ColorID::Black); //dodavanje nove linije na graf
	void dodajFunkciju(gui::CoordType*&& x, gui::CoordType*&& y, const size_t broj_tacaka, td::ColorID = td::ColorID::Black);



	void zoomToFit(gui::CoordType limitWidth = 0, gui::CoordType limitHeight = 0);

	void setAxisColor(td::ColorID boja);

};

