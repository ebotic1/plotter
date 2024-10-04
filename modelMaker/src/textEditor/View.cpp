#include "View.h"
#include "../globalEvents.h"
#include "../mainWindow.h"


EquationView::EquationView():
	_hl(1)
{
	_hl << textMain;
	setLayout(&_hl);
}

void TextEditorView::getModelNode(modelNode &model)
{
	model.processCommands(equationView.textMain.getText());
}


TextEditorView::TextEditorView() :
	_hl(1)
{
	_hl << equationView;
	setLayout(&_hl);
}



