#include "View.h"


TextEditorView::TextEditorView():
	splitter(gui::SplitterLayout::Orientation::Vertical)
{
	splitter.setContent(equationView, logView);



	setLayout(&splitter);
}