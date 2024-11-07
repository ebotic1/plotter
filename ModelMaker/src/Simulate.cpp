#include "MainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include <type_traits>
#include <chrono>
#include "textEditor/View.h"
#include "BaseView.h"
#include "ModelSettings.h"
#include "DataDrawer.h"



//double NR solver (for NL and WLS)
inline int solveNR(sc::IDblSolver* pSolver, const td::String& modelStr)
{
    if (!pSolver->init(modelStr, sc::IDblSolver::SourceType::Memory))
    {
        std::cout << "Error! Solver Init FAILED" << std::endl;
        std::cout << pSolver->getLastErrorStr() << std::endl;
        return -10;
    }
    
    auto solStatus = pSolver->solve();
    if (solStatus != sc::Solution::OK)
    {
        std::cout << "ERROR! Cannot solve the problem" << std::endl;
        std::cout << "SolutionStatus=" << pSolver->getSolutionStatusStr() << std::endl;
        return -11;
    }
    
    std::cout << "Problem solved in "<< pSolver->getIterationsNo() << " iterations. Precision = " << pSolver->getSolvedPrecision() << std::endl;
    
    
    int nVar = pSolver->getNumberVariables();
    int nPar = pSolver->getNumberParameters();
    std::cout << "Problem solved in "<< pSolver->getIterationsNo() << " iterations. Precision = " << pSolver->getSolvedPrecision() << std::endl;

    std::cout << "No of Variables: " << nVar << std::endl << "---------------------------" << std::endl;
    std::cout << "SOLUTION_DATA" << std::endl << "---------------------------" << std::endl;

    double* pVar = pSolver->getVariablesPtr();
    
    for (int i=0; i< nVar; i++, pVar++)
    {
        std::cout << i+1 << ". " << pSolver->getVariableName(i) <<"=" << *pVar << std::endl;
    }
    return 0;
}


//complex NR solver (for NL and WLS)
inline int solveNR(sc::ICmplxSolver* pSolver, const td::String& modelStr)
{
    if (!pSolver->init(modelStr, sc::ICmplxSolver::SourceType::Memory))
    {
        std::cout << "Error! Solver Init FAILED" << std::endl;
        std::cout << pSolver->getLastErrorStr() << std::endl;
        return -10;
    }
    
    auto solStatus = pSolver->solve();
    if (solStatus != sc::Solution::OK)
    {
        std::cout << "ERROR! Cannot solve the problem" << std::endl;
        std::cout << "SolutionStatus=" << pSolver->getSolutionStatusStr() << std::endl;
        return -11;
    }
    
    std::cout << "Problem solved in "<< pSolver->getIterationsNo() << " iterations. Precision = " << pSolver->getSolvedPrecision() << std::endl;
    
    
    int nVar = pSolver->getNumberVariables();
    int nPar = pSolver->getNumberParameters();
    std::cout << "Problem solved in "<< pSolver->getIterationsNo() << " iterations. Precision = " << pSolver->getSolvedPrecision() << std::endl;

    std::cout << "No of Variables: " << nVar << std::endl << "---------------------------" << std::endl;
    std::cout << "SOLUTION_DATA" << std::endl << "---------------------------" << std::endl;

    td::cmplx* pVar = pSolver->getVariablesPtr();
    
    for (int i=0; i< nVar; i++, pVar++)
    {
        std::cout << i+1 << ". " << pSolver->getVariableName(i) <<"=" << *pVar << std::endl;
    }
    return 0;
}



#ifdef MU_DEBUG
#define TEST_SIMULATION

#ifdef TEST_SIMULATION
fo::fs::path getRealPath()
{
    fo::fs::path homePath;
    mu::getHomePath(homePath);
    fo::fs::path toRet(homePath / "other_bin/TestData/SymbComp/real");
    return toRet;
}

fo::fs::path getCmplxPath()
{
    fo::fs::path homePath;
    mu::getHomePath(homePath);
    fo::fs::path toRet(homePath / "other_bin/TestData/SymbComp/cmplx");
    return toRet;
}

td::String getRealName(const char* fn)
{
    fo::fs::path fnPath(getRealPath() / fn);
    td::String toRet = fnPath.string();
    return toRet;
}

td::String getCmplxName(const char* fn)
{
    fo::fs::path fnPath(getCmplxPath() / fn);
    td::String toRet = fnPath.string();
    return toRet;
}

td::String getRealXMLData(const char* fn)
{
    td::String fullName = getRealName(fn);
    td::String strContent;
    fo::loadBinaryFileAtOnce(fullName, strContent);
    return strContent;
}

td::String getCmplxXMLData(const char* fn)
{
    td::String fullName = getCmplxName(fn);
    td::String strContent;
    fo::loadBinaryFileAtOnce(fullName, strContent);
    return strContent;
}

#endif //TEST_SIMULATION
#endif //MU_DEBUG

using LogType = LogView::LogType;


template<typename solverType, typename resultType>
class SolutionBuffer : public sc::ISolutionBuffer {
	const std::vector<ModelSettings::FunctionDesc> &functions;
	const std::shared_ptr<LogView> logger;
	solverType *solver;
	std::chrono::steady_clock::time_point startTime;
	int cnt = 0, maxPoints;

	std::vector<std::pair<double*, resultType*>> storePointers, storePointersImag;
	std::vector<DataDraw::FunctionDesc> outFuncs, outDataSet;

	const td::String& modelName;

	std::map<td::String, double*> NameToStorePtrCont;
	double* getPointerToVar(const td::String &varName) {
		if (NameToStorePtrCont.contains(varName))
			return NameToStorePtrCont[varName];
		else
			return NameToStorePtrCont[varName] = new double[maxPoints];
	}


	inline td::String addCmplxTag(const td::String& name, const bool& isComplex) {
		if (isComplex)
			return td::String(name) += ".imag";
		else
			return td::String(name) += ".real";
	}

	inline td::String getCmplxString(td::cmplx number) {
		cnt::StringBuilderSmall str;
		str << number;
		td::String result;
		str.getString(result);
		return result;
	}

public:
	SolutionBuffer(const td::String &modelName, const std::shared_ptr<LogView> logger, bool initSucess, const std::vector<ModelSettings::FunctionDesc> &functions, int maxPoints, solverType *solver):
		functions(functions),
		logger(logger),
		solver(solver),
		maxPoints(maxPoints),
		modelName(modelName),
		startTime(std::chrono::steady_clock::now())
	{

		if(!initSucess)
			return;

		constexpr bool isComplex = std::is_same<resultType, td::cmplx>::value;
		cnt::SafeFullVector<td::INT4> symbs;
		solver->getOutSymbols(symbs);

		std::map<td::String, td::INT4> outVars;
		
		for (const auto& index : symbs) 
			outVars[solver->getSymbolName(index)] = index;
		if (int index = solver->getParamIndex("t"); index >= 0)
			outVars["t"] = index;
	
		std::map<td::String, td::INT4>::iterator itX, itY, end = outVars.end();
		td::String xAxis, yAxis;
		int paramIntX, paramIntY;




		for (const auto& funD : functions) {
			if (funD.xAxis.cCompare("0") == 0 && funD.yAxis.cCompare("0") == 0)
				continue;


#ifdef MU_DEBUG
			td::String log2("Processing function: ");
			log2 += funD.name;
			logger->appendLog(log2, LogType::warning);
#endif // MU_DEBUG

			if constexpr (!isComplex) {
				if (funD.Xcomplex || funD.Ycomplex)
					continue;
			}
			else {
				xAxis = addCmplxTag(funD.xAxis, funD.Xcomplex);
				yAxis = addCmplxTag(funD.yAxis, funD.Ycomplex);
			}
				
			
			itX = outVars.find(funD.xAxis);
			itY = outVars.find(funD.yAxis);

			paramIntX = (itX == end) ? solver->getParamIndex(funD.xAxis.c_str()) : -1;
			paramIntY = (itY == end) ? solver->getParamIndex(funD.yAxis.c_str()) : -1;

			if (itX != end && itY != end) { //dva niza
				if constexpr (!isComplex) {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(funD.xAxis), getPointerToVar(funD.yAxis), maxPoints, funD.xAxis, funD.yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(funD.xAxis), getPointerToVar(funD.yAxis), maxPoints, funD.xAxis, funD.yAxis));
				}
				else {

					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(xAxis), getPointerToVar(yAxis), maxPoints, xAxis, yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(xAxis), getPointerToVar(yAxis), maxPoints, xAxis, yAxis));
				}
				continue;
			}




			if (itX == end && paramIntX >= 0 && funD.yAxis.cCompare("0") == 0) { // parametar u x
				if constexpr (!isComplex) {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, solver->getParamsPtr() + paramIntX, nullptr, 1, funD.xAxis, ""));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, solver->getParamsPtr() + paramIntX, nullptr, 1, funD.xAxis, ""));
				}
				else {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntX])));
				}
				continue;
			}


			if (itY == end && paramIntY >= 0 && funD.xAxis.cCompare("0") == 0) { // parametar u y
				if constexpr (!isComplex) {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, nullptr, solver->getParamsPtr() + paramIntY, 1, "", funD.yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, solver->getParamsPtr() + paramIntY, 1, "", funD.yAxis));
				}
				else {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntY]))); 
				}
				continue;
			}

			if (itY != end && funD.xAxis.cCompare("0") == 0) { // jedan niz

				if constexpr (!isComplex)
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, getPointerToVar(funD.yAxis), maxPoints, "", funD.yAxis));
				else {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, getPointerToVar(xAxis), maxPoints, "", yAxis));
				}
				continue;
			}


			td::String log("Discarding function/dataset ");
			log += funD.name;
			logger->appendLog(log, LogType::warning);


			
		}

	

		td::String nameWithoutExtension;
		for (const auto& nameAndPtr : NameToStorePtrCont) {
			if (nameAndPtr.first.cCompare("t") == 0) {
				storePointers.push_back({ nameAndPtr.second, solver->getParamsPtr() + outVars[nameAndPtr.first]});
				continue;
			}

			if constexpr (!isComplex)
				storePointers.push_back({ nameAndPtr.second, solver->getVariablesPtr() + outVars[nameAndPtr.first]});
			else {
				nameWithoutExtension = nameAndPtr.first;
				nameWithoutExtension.reduceSize(5);
				if(nameAndPtr.first.endsWith(".real"))
					storePointers.push_back({ nameAndPtr.second, solver->getVariablesPtr() + outVars[nameWithoutExtension] });
				else
					storePointersImag.push_back({ nameAndPtr.second, solver->getVariablesPtr() + outVars[nameWithoutExtension] });
			}
		}

		

	}



	void put() override
	{
		if constexpr (std::is_same<resultType, td::cmplx>::value) {
			for (int i = 0; i < storePointers.size(); ++i)
				storePointers[i].first[cnt] = (storePointers[i].second)->real();
			for (int i = 0; i < storePointersImag.size(); ++i)
				storePointersImag[i].first[cnt] = (storePointers[i].second)->imag();
		}
		else {
			for (int i = 0; i < storePointers.size(); ++i)
				storePointers[i].first[cnt] = *storePointers[i].second;
		}

		++cnt;
	}

	void finalize(Status status) override
	{
		cnt::StringBuilderSmall str;
		td::String log;
		if (status == sc::ISolutionBuffer::Status::Error) {
			str << "Simulation stopped with error: " << solver->getLastErrorStr();
			str.getString(log);
			logger->appendLog(log, LogType::error);
			return;
		}
		else {
			std::chrono::duration<double> d = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - startTime);
			str << "Simulation finished sucessfully in " << d.count() << " seconds.";
			str.getString(log);
			logger->appendLog(log, LogType::info);

		}

		td::String nameAndType(modelName);
		nameAndType += "-graph";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outFuncs, DataDraw::Type::graph);

		nameAndType = modelName;
		nameAndType += "-table";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outDataSet, DataDraw::Type::table);
	
	}
};


SolutionBuffer(const td::String &, const std::shared_ptr<LogView>, bool, const std::vector<ModelSettings::FunctionDesc> &, int, sc::IDblSolver *) -> SolutionBuffer<sc::IDblSolver, double>;
SolutionBuffer(const td::String &, const std::shared_ptr<LogView>, bool, const std::vector<ModelSettings::FunctionDesc> &, int, sc::ICmplxSolver *) -> SolutionBuffer<sc::ICmplxSolver, td::cmplx>;


enum class EquationTypes { NR, DAE, ODE, WLS };



int MainWindow::simulate(ViewForTab *tab)
{
	const auto& logView = tab->getLog();
	bool error;
	const modelNode &model = tab->getModelNode(error);
	if(error){
		logView->appendLog(tr("simulationFailed"), LogType::error);
		return -1;
	}
	
	bool isComplex;
	if(auto it = model._attribs.find("domain"); it == model._attribs.end())
		isComplex = false;
	else{
		isComplex = (it->second.cCompareNoCase("cmplx") == 0 || it->second.cCompareNoCase("complex") == 0) ? true : false;
		if(!isComplex && it->second.cCompareNoCase("real") != 0)
			logView->appendLog("model attribute 'domain' is not 'real' or 'complex', assuming real", LogType::warning);
	}


    const char* testFileName = nullptr;
    
	EquationTypes equationType;
	if(auto it = model._attribs.find("type"); it == model._attribs.end()){
		logView->appendLog("model does not have attribute 'type', assuming 'NR'(nonlinear equations)", LogType::warning);
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
			logView->appendLog(msg, LogType::error);
			logView->appendLog("Supported types are: NR, DAE, ODE, WLS", LogType::info);
			return -2;
		}
	}

	

	double startTime, stepTime, endTime;
	unsigned int maxIter;
	tab->getTimes(startTime, endTime, stepTime, maxIter);

	const auto &funcs = tab->getFunctions();

	td::String modelStr;
	model.printNodeToString(modelStr);	

	
#ifdef MU_DEBUG
#ifdef MU_MACOS
	td::String mPath = "/Volumes/RAMDisk/modelForSimulator.xml";
	model.printNode(mPath); //u slucaju problema da se moze pogledati finalni generisani model koji je poslan solveru
#endif
	td::String mPath = "./modelForSimulator.xml";
	model.printNode(mPath);
#endif


	//mPath = "/home/bots/Desktop/model-Solver/real/ACGenWith1Load_WithLimit_Comp_DAE_RK4.xml";

	auto initSimulation = [&](auto s)
    {
		td::String err;
		bool initSucess = false;
		if constexpr (std::is_same<decltype(s), sc::IDblSolver*>::value)
        {
			switch (equationType) {
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
            initSucess = s->init(modelStr, sc::IDblSolver::SourceType::Memory);
		}
		else if constexpr (std::is_same<decltype(s), sc::ICmplxSolver*>::value)
        {
			switch (equationType){
            case EquationTypes::NR:
                s = sc::createCmplxNRSolver(maxIter);
				break;
			case EquationTypes::ODE:
                    logView->appendLog("Complex ODE solver will be implemented in the future", LogType::error);
                    return -1;
					//s = sc::createCmplxODESolver();
				break;
			case EquationTypes::DAE:
				//s = sc::createCmplxDAESolver(maxIter);
                logView->appendLog("Complex DAE solver will be implemented in the future", LogType::error);
                return -1;
			case EquationTypes::WLS:
                s = sc::createCmplxWLSSolver(maxIter);
				break;
			}
			initSucess = s->init(modelStr, sc::ICmplxSolver::SourceType::Memory);
		}
	else
		static_assert(false, "wrong pointer type for initSimulation");

		bool useAutoFuncs = false;
		std::vector<ModelSettings::FunctionDesc> autoFuncs;
		int size = (equationType == EquationTypes::NR || equationType == EquationTypes::WLS) ? 1 : 1 + std::abs(startTime - endTime) / stepTime;

		if (funcs.empty() && initSucess) {
			useAutoFuncs = true;
			cnt::SafeFullVector<td::INT4> symbs;

			s->getOutSymbols(symbs);
			int timeIndex = s->getParamIndex("t");
			
			if (timeIndex < 0) 
				for (const auto& symIndex : symbs)
					autoFuncs.push_back(ModelSettings::FunctionDesc(ModelSettings::FunctionDesc::Type::points, s->getSymbolName(symIndex), s->getSymbolName(symIndex), "0"));
			else 
				for (const auto& symIndex : symbs)
					autoFuncs.push_back(ModelSettings::FunctionDesc(ModelSettings::FunctionDesc::Type::graph, s->getSymbolName(symIndex), s->getSymbolName(symIndex), "t"));

			if (autoFuncs.empty())
				logView->appendLog("No out variables found. You must add 'out=true' attribute to a single variable or the variable declaration tag for them to be visible to the plotter", LogType::warning);
		}

		auto buffer = new SolutionBuffer(tab->getName(), logView, initSucess, useAutoFuncs ? autoFuncs : funcs, size, s);

		if(initSucess){
			if(equationType == EquationTypes::DAE || equationType == EquationTypes::ODE)
				s->solve(startTime, stepTime, endTime, buffer, 0);
			if(equationType == EquationTypes::NR || equationType == EquationTypes::WLS){
				s->solve();
				buffer->put();
				err = s->getLastErrorStr();
				buffer->finalize(err.isNull() ? sc::ISolutionBuffer::Status::SuccessfullyCompleted : sc::ISolutionBuffer::Status::Error);
			}
			return 0;
		}

		buffer->finalize(sc::ISolutionBuffer::Status::Error);

		return -1;

	};


	if (isComplex) {
		sc::ICmplxSolver* s = nullptr;
		initSimulation(s);
	}
	else {
		sc::IDblSolver* s = nullptr; 
		initSimulation(s);
	}
    
    return 0;

}

