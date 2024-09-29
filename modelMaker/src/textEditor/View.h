#include "gui/View.h"
#include "gui/TextEdit.h"
#include "gui/VerticalLayout.h"
#include "gui/HorizontalLayout.h"
#include "gui/Label.h"
#include "../../../Canvas/inc/canvas.h"
#include "gui/SplitterLayout.h"
#include "../baseView.h"

class TextEditorView : public BaseViewForTab {

	gui::SplitterLayout splitter;


public:

	TextEditorView();
	
};