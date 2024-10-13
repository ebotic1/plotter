#include "mainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include "textEditor/View.h"
#include "baseView.h"
#include "modelSettings.h"


template<typename solverType, typename resultType>
class SolutionBuffer : public sc::ISolutionBuffer {
	const std::vector<ModelSettings::FunctionDesc> &functions;
	const LogView *logger;
	resultType a;
	solverType *solver;
public:
	SolutionBuffer(const LogView *logger, std::vector<ModelSettings::FunctionDesc> functions, solverType *solver):
		functions(functions),
		logger(logger),
		solver(solver)
	{
		
		

	}



	void put() override
	{
		int x = 0;
		
	}

	void finalize(Status status) override
	{

		int x = 0;

	}
};


SolutionBuffer(const LogView *, const std::vector<ModelSettings::FunctionDesc>, sc::IDblSolver *) -> SolutionBuffer<sc::IDblSolver, double>;
SolutionBuffer(const LogView *, const std::vector<ModelSettings::FunctionDesc>, sc::ICmplxSolver *) -> SolutionBuffer<sc::ICmplxSolver, td::cmplx>;




using LogType = LogView::LogType;
enum class EquationTypes{NR, DAE, ODE, WLS};

void MainWindow::simulate(ViewForTab *tab)
{
	const auto& logView = tab->getLog();
	bool error;
	const modelNode &model = tab->getModelNode(error);
	if(error){
		logView.appendLog(tr("simulationFailed"), LogType::error);
		return;
	}
	
	//return; //izbridaati
	bool isComplex;
	if(auto it = model.attribs.find("domain"); it == model.attribs.end())
		isComplex = false;
	else{
		isComplex = (it->second.cCompareNoCase("cmplx") == 0 || it->second.cCompareNoCase("complex") == 0) ? true : false;
		if(!isComplex && it->second.cCompareNoCase("real") != 0)
			logView.appendLog("model attribute 'domain' is not 'real' or 'complex', assuming real", LogType::warning);
	}


	EquationTypes equationType;
	if(auto it = model.attribs.find("type"); it == model.attribs.end()){
		logView.appendLog("model does not have attribute 'type', assuming 'NR'(nonlinear equations)", LogType::warning);
		equationType = EquationTypes::NR;
	}else{
		if(it->second.cCompareNoCase("NR") == 0)
			equationType = EquationTypes::NR;
		else if(it->second.cCompareNoCase("DAE") == 0)
			equationType = EquationTypes::DAE;
		else if(it->second.cCompareNoCase("ODE") == 0)
			equationType = EquationTypes::ODE;
		else if(it->second.cCompareNoCase("WLS") == 0)
			equationType = EquationTypes::WLS;
		else{
			td::String msg("attribute 'type' has unsupported value ");
			msg += it->second;
			msg += ", simulation stopping";
			logView.appendLog(msg, LogType::error);
			logView.appendLog("Supported types are: NR, DAE, ODE, WLS", LogType::info);
			return;
		}
	}

	

	double startTime, stepTime, endTime;
	unsigned int maxIter;
	tab->getTimes(startTime, endTime, stepTime, maxIter);

	const auto &funcs = tab->getFunctions();

	td::String modelStr;
	model.printNodeToString(modelStr);	

	td::String err;


	if (!isComplex) {
		sc::IDblSolver *s;
		switch (equationType){
		case EquationTypes::NR:
			s = sc::createDblNRSolver(maxIter);
			break;
		case EquationTypes::ODE:
			s = sc::createDblODESolver();
			break;
		case EquationTypes::DAE:
			s = sc::createDblDAESolver(maxIter);
			break;
		case EquationTypes::WLS:
			s = sc::createDblWLSSolver(maxIter);
			break;
		}

		s->init(modelStr, sc::IDblSolver::SourceType::Memory);
		s->solve(startTime, stepTime, endTime, new SolutionBuffer(&logView, funcs, s), 0);




		err = s->getLastErrorStr();
		double a = 4444;
		s->getVariableValue("x", a);
		double r[599];
		s->getResults(r);
		int c = 3;

		auto kk = s->getVariablesPtr();
		//double d = *kk;

		int x = 8;


	}else{
		sc::ICmplxSolver *s;
		
		switch (equationType){
		case EquationTypes::NR:
			s = sc::createCmplxNRSolver(maxIter);
			break;
		case EquationTypes::ODE:
			s = sc::createCmplxODESolver();
			break;
		case EquationTypes::DAE:
			s = sc::createCmplxDAESolver(maxIter);
			break;
		case EquationTypes::WLS:
			s = sc::createCmplxWLSSolver(maxIter);
			break;
		}


		s->init(modelStr, sc::ICmplxSolver::SourceType::Memory);
		s->solve(startTime, stepTime, endTime, nullptr);



		err = s->getLastErrorStr();



	}
	


	if(!err.isNull()){
		td::String str("Simulation error: ");
		str += err;
		logView.appendLog(str, LogType::error);
	}


}

