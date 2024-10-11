#include "View.h"
#include "../globalEvents.h"
#include "../mainWindow.h"
#include <gui/FileDialog.h>
#include <fstream>




TextEditorView::TextEditorView() :
	_hl(1),
	textMain(this)
{
	_hl << textMain;
	setLayout(&_hl);
}

void TextEditorView::getModel(modelNode &model)
{
	model.clear();
	model.processCommands(textMain.getText());
}



bool TextEditorView::save(const td::String &path, const td::String &settingsString)
{
	std::ofstream out(path.c_str());
	out << settingsString << "\n";
	out << textMain.getText();
	if(out.fail()){
		showAlert(tr("saveFile"), tr("cantSaveFile"));
		return false;
	}
	return true;
}

void TextEditorView::saveAs(const td::String &settingsString, td::String *newPath)
{

	td::String *settingsStringPtr = new td::String(settingsString);
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
	modelChanged();
    if(path.endsWith(".txt")){
		std::ifstream in(path.c_str());
		cnt::StringBuilder s;
		char buffer[400];

		if(!in)
			return false;

		const char *found;
		const int len = std::strlen("Model:");
		settingsString = "";
		bool firstLoop = true;
		while(in){//pronalazenje "model:" tag-a i stavljanje svog sadrzaja prije njega u settingsString
			in.read(buffer + len, sizeof(buffer) - len);
			found = std::strstr(buffer + len, "Model:");
			if(found){
				if(firstLoop)
					settingsString += td::String(buffer + len, found - buffer - len);
				else
					settingsString += td::String(buffer, found - buffer);

				s.appendString(found, in.gcount()+len-(found - buffer));
				break;
			}
			else{
				if(in.gcount() != sizeof(buffer) - len){
					showAlert(tr("error"), tr("noModelTagFound"));
					if(firstLoop)
						settingsString += td::String(buffer + len, in.gcount());
					else
						settingsString += td::String(buffer, in.gcount() + len);

					return settingsString.find("preProc(") != -1;
				}
				if(firstLoop){
					settingsString += td::String(buffer + len, sizeof(buffer) - 2*len);
					firstLoop = false;
				}else
					settingsString += td::String(buffer, sizeof(buffer) - len);
			}
			
			memmove(buffer, buffer + sizeof(buffer) - len, len);

		}


		while(in){//model: je pronaden sad se samo ocita ostatak fajla
			in.read(buffer, sizeof(buffer));
			s.appendString(buffer, in.gcount());
		}

		td::String code;
		s.getString(code);

		textMain.setText(code);
		
		return true;
	}
	if(path.endsWith(".xml")){
		try {
			td::String s;
			GlobalEvents::getMainWindowPtr()->getModelFromTabOrFile(path).prettyPrint(s);
			textMain.setText(s);
			return true;
		}
		catch (MainWindow::exceptionCantAccessFile&) {
			showAlert(tr("error"), tr("cantLoadXML"));
		}
		catch (modelNode::exceptionInvalidBlockName& name) {
			cnt::StringBuilderSmall s;
			s << "Unrecognized block \"" << name.message << "Cant load model";
			td::String msg;
			s.getString(msg);
			showAlert(tr("error"), msg);
			return false;
		}
		catch (...) {
			return false;
		}
	}
	return false;
}
