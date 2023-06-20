#include "gui/ViewScroller.h"
#include "gui/VerticalLayout.h"
#include "gui/HorizontalLayout.h"

class scroller : public gui::ViewScroller {
	scroller();
private:
	gui::VerticalLayout* vert;
};