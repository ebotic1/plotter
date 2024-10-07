#include "View.h"
#include "../globalEvents.h"
#include "../mainWindow.h"
#include <gui/FileDialog.h>
#include <fstream>


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



bool TextEditorView::save(const td::String &path, const td::String &settingsString)
{
	std::ofstream out(path.c_str());
	out << settingsString << "\n";
	out << equationView.textMain.getText();
	if(out.fail()){
		showAlert(tr("saveFile"), tr("cantSaveFile"));
		return false;
	}
	return true;
}

void TextEditorView::saveAs(const td::String &settingsString, td::String *newPath)
{

	td::String *settingsStringPtr = new td::String(std::move(settingsString));
	auto &s = *new gui::SaveFileDialog (this, tr("saveFile"), ".txt");
	s.openModal([newPath, this, settingsStringPtr](gui::FileDialog *dialog){
		auto path = dialog->getFileName();
		if(!path.isNull()){
			*newPath = path;
			save(path, *settingsStringPtr);
		}
		delete settingsStringPtr;
	});
	
}



bool TextEditorView::openFile(const td::String &path, td::String &settingsString)
{
    if(path.endsWith(".txt")){
		std::ifstream in(path.c_str());
		cnt::StringBuilder s;

		in >> settingsString;
		in.get();
		if(!in)
			return false;

		char buffer[3];
		while(in){
			in.read(buffer, sizeof(buffer));
			s << buffer;
		}

		td::String code;
		s.getString(code);

		equationView.textMain.setText(code);
		
		return true;
	}
	if(path.endsWith(".xml")){

	}
	return false;
}
