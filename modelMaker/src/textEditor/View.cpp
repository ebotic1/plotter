#include "View.h"
#include "../globalEvents.h"
#include "../mainWindow.h"


EquationView::EquationView():
	_hl(1)
{
	_hl << textMain;
	setLayout(&_hl);
}



TextEditorView::TextEditorView() :
	_hl(1)
{
	_hl << equationView;
	setLayout(&_hl);
}

void TextEditorView::getModel(modelNode &model)
{
	model.clear();
	model.processCommands(equationView.textMain.getText());
}
