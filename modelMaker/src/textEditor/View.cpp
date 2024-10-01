#include "View.h"
#include <sc/SCLib.h>
#include "../globalEvents.h"
#include "../mainWindow.h"

using LogType = LogView::LogType;

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

TextEditorView::TextEditorView() : splitter(gui::SplitterLayout::Orientation::Vertical){
    splitter.setContent(equationView, logView);
    setLayout(&splitter);
}


void TextEditorView::simulate(double startTime, double endTime, double stepTime)
{
	modelNode model;
	getModelNode(model);

	if(model["name"].length() != 0){
		GlobalEvents::getMainWindowPtr()->changeTabName(model["name"]);
	}

	bool isComplex = false;
	isComplex = (model["domain"].cCompareNoCase("cmplx") == 0 || model["domain"].cCompareNoCase("complex") == 0) ? true : false;

	auto type = model["type"];

	if(!(type.cCompareNoCase("DAE") == 0 || type.cCompareNoCase("ODE") == 0 || type.cCompareNoCase("WLS") == 0)){
		if(type.length() != 0){
			td::String msg(tr("unsupportedType"));
			msg += type;
			msg += "\"";
			logView.appendLog(msg, LogType::error);
			logView.appendLog(tr("supportedModelsInfo"), LogType::info);
			return;
		}
	
		logView.appendLog(tr("noTypeFound"), LogType::warning);
		type = "DAE";
		
	}

	if(type.cCompareNoCase("DAE") == 0){
		if(isComplex){
			auto a = sc::createCmplxDAESolver(10);	
			//a->solve()
		}
	}

	logView.appendLog(model["name"].c_str(), LogType::info);
	

	//auto solverPtr = (isComplex) ? (void*)(new sc::ICmplxSolver *) : (void*)(new sc::IDblSolver *);

	//delete solverPtr;	
}
