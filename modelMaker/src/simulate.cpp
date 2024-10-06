#include "mainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include "textEditor/View.h"
#include "baseView.h"


class SolutionBuffer : public sc::ISolutionBuffer {

public:
	void put() {

	}
	void finalize(Status status) {

	}
};

using LogType = LogView::LogType;

void MainWindow::simulate(ViewForTab *tab)
{
	
	/*
	const modelNode &model = tab->getModelNode();
	const auto& logView = tab->getLog();


	if (model["name"].length() != 0) {
		GlobalEvents::getMainWindowPtr()->changeTabName(model["name"]);
	}

	bool isComplex = false;
	isComplex = (model["domain"].cCompareNoCase("cmplx") == 0 || model["domain"].cCompareNoCase("complex") == 0) ? true : false;

	auto type = model["type"];

	if (!(type.cCompareNoCase("DAE") == 0 || type.cCompareNoCase("ODE") == 0 || type.cCompareNoCase("WLS") == 0 || type.cCompareNoCase("NR") == 0)) {
		if (type.length() != 0) {
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

	double startTime, stepTime, endTime;
	unsigned int maxIter;
	tab->getTimes(startTime, endTime, stepTime, maxIter);

	//model.printNode("D:/deltete/fajl.xml");
	td::String modelStr;
	model.printNodeToString(modelStr);
	smece sma;
	if (type.cCompareNoCase("DAE") == 0) {
		if (!isComplex) {
			auto a = sc::createDblDAESolver(maxIter);
			a->init(modelStr, sc::IDblSolver::SourceType::Memory);
			a->solve(startTime, stepTime, endTime, &sma);
			cnt::SafeFullVector<td::INT4> v;
			a->getOutSymbols(v);
			int x = 3;
		}
	}

	if (type.cCompareNoCase("NR") == 0) {
		if (!isComplex) {
			auto a = sc::createDblNRSolver(10);
			a->init(modelStr, sc::IDblSolver::SourceType::Memory);
			a->solve(startTime, stepTime, endTime, &sma);
			cnt::SafeFullVector<td::INT4> v;
			a->getOutSymbols(v);
			a->
				int x = 3;
		}
	}

	logView.appendLog(model["name"].c_str(), LogType::info);	

	*/
	
}
