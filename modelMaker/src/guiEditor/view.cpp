#include "view.h"
#include "globals.h"
#include "View.h"


kanvas* ptr;

GraphicalEditorView::GraphicalEditorView() : spliter(gui::SplitterLayout::Orientation::Horizontal), scroller(gui::ViewScroller::Type::ScrollAndAutoHide, gui::ViewScroller::Type::ScrollAndAutoHide) {
	ptr = &_canvas;
	scroller.setContentView(&_canvas);
	spliter.setContent(scroller, props);
	setLayout(&spliter);
}


void globals::refreshCanvas() {
	ptr->reDraw();
}

