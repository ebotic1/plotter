#include "MainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include <type_traits>
#include <chrono>
#include "textEditor/View.h"
#include "BaseView.h"
#include "ModelSettings.h"
#include "DataDrawer.h"
#include <td/Timer.h>
#include <dp/IDatabase.h>

/*
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

*/

using LogType = LogView::LogType;

template <template <typename> class SolverTemplate, typename T>
class SolutionBuffer : public sc::ISolutionBuffer
{
	static constexpr bool isComplex = std::is_same<T, td::cmplx>::value;

	const std::vector<ModelSettings::FunctionDesc> &functions;
	const std::shared_ptr<LogView> logger;
	std::shared_ptr<Results> _results;
    SolverTemplate<T> *solver;

    dp::IDataSet* _pDS;
	std::chrono::steady_clock::time_point startTime;
	

	const td::String& modelName;
	std::vector<std::pair<T*, td::String>> _valuePtrAndName;
	int _indexOfTimeParameter;

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
    SolutionBuffer(const td::String &modelName, const std::shared_ptr<LogView> logger, std::shared_ptr<Results> resultTable, \
	const std::vector<ModelSettings::FunctionDesc> &functions, SolverTemplate<T>* solver, bool initSucess)
    : functions(functions)
    , logger(logger)
    , solver(solver)
    , modelName(modelName)
    , startTime(std::chrono::steady_clock::now())
    , _results(resultTable)
    {
        
        if(!initSucess)
            return;
        
        T* _pSymbolValues = solver->getSymbolValuesPtr();
        
		cnt::SafeFullVector<td::INT4> outSymbols;
        solver->getOutSymbols(outSymbols);
	

        _indexOfTimeParameter = solver->getParamIndex("t");
        if (_indexOfTimeParameter >= 0)
			_valuePtrAndName.emplace_back(solver->getParamsPtr() + _indexOfTimeParameter, "t");

		for (const auto& index : outSymbols){
			if constexpr(isComplex){
				_valuePtrAndName.emplace_back((_pSymbolValues + index), addCmplxTag(solver->getSymbolName(index), false));
				_valuePtrAndName.emplace_back((_pSymbolValues + index), addCmplxTag(solver->getSymbolName(index), true));
			}
			else
				_valuePtrAndName.emplace_back(_pSymbolValues + index, solver->getSymbolName(index));
		}

		size_t columnCnt = _valuePtrAndName.size();
        _pDS = _results->getDataSet();
        if (_pDS) //Provjera da li je potrebno mijenjati kolone tabele
        {
            size_t nCols = _pDS->getNumberOfCols();
            if (nCols != columnCnt)
            {
                _pDS = nullptr;
            }
            else
            {
				for(int i = 0; i<columnCnt; ++i){
					if(_valuePtrAndName[i].second != _pDS->getColName(i)){
						_pDS = nullptr;
						break;
					}
				}
			}
		}
                         
        if (!_pDS)
        {
            //create DataSet
            _pDS = dp::createConnectionlessDataSet(dp::IDataSet::Size::Medium);
            dp::DSColumns cols(_pDS->allocBindColumns(columnCnt));
            
			for(int i = 0; i<columnCnt; ++i)
				cols << _valuePtrAndName[i].second.c_str() << td::real8;

            _pDS->execute();
        }
        else
        {
            _pDS->removeAll();
        }
        

	}

	void put() override
	{
		
		auto& row = _pDS->getEmptyRow();

		if constexpr(isComplex){
			int i = 0;
			if(_indexOfTimeParameter > 0){ //ovo je poprilicno lose rijesenje ali std::complex nije namijenjen za ove svrhe 
				row[i] = _valuePtrAndName[i].first->real();
				++i;
			}

			for(; i<_valuePtrAndName.size(); ++i){
				row[i] = _valuePtrAndName[i].first->real();
				++i;
				row[i] = _valuePtrAndName[i].first->imag();
			}
				
		}
		else{
			for(int i = 0; i<_valuePtrAndName.size(); ++i)
				row[i] = *(_valuePtrAndName[i].first);
		}
		_pDS->push_back();
		
	}

	void finalize(Status status) override
	{

		cnt::StringBuilderSmall str;
		td::String log;
		if (status == sc::ISolutionBuffer::Status::Error) {
			str << "Simulation stopped with error: " << solver->getLastErrorStr();
			str.getString(log);
			logger->appendLog(log, LogType::Error);
			return;
		}
		else {
			std::chrono::duration<double> d = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - startTime);
			str << "Simulation finished sucessfully in " << d.count() << " seconds.";
			_results->show(_pDS);
			str.getString(log);
			logger->appendLog(log, LogType::Info);
		}




		cnt::PushBackVector<td::String> names;
		cnt::PushBackVector<cnt::PushBackVector<double>> data;
		for(const auto &valAndName : _valuePtrAndName)
			names.push_back(valAndName.second);	

		_pDS->getFPValues(names, data);

		std::map<td::String, double*> NameAndDataPtr; //za lakse pretrazivanje. Moglo bi se izbaciti
		for(int i = 0; i<_valuePtrAndName.size(); ++i)
			NameAndDataPtr[_valuePtrAndName[i].second] = &(data[i][0]);

		size_t size = (data.size() > 0) ? data[0].size() : 0;

		std::vector<DataDraw::FunctionDesc> outFuncs, outDataSet;
		decltype(NameAndDataPtr)::iterator itX, itY, end = NameAndDataPtr.end();
		td::String xAxis, yAxis;
		int paramIntX, paramIntY;

		for(const auto &funD : functions){//zahtjevi za crtane grafova/tabela
			if (funD.xAxis.cCompare("0") == 0 && funD.yAxis.cCompare("0") == 0)
				continue;

			if constexpr (!isComplex) {
				if (funD.Xcomplex || funD.Ycomplex)
					continue;
				itX = NameAndDataPtr.find(funD.xAxis);
				itY = NameAndDataPtr.find(funD.yAxis);
			}
			else {
				xAxis = addCmplxTag(funD.xAxis, funD.Xcomplex);
				yAxis = addCmplxTag(funD.yAxis, funD.Ycomplex);

				itX = NameAndDataPtr.find(xAxis);
				itY = NameAndDataPtr.find(yAxis);
			}



			paramIntX = (itX == end) ? solver->getParamIndex(funD.xAxis.c_str()) : -1;
			paramIntY = (itY == end) ? solver->getParamIndex(funD.yAxis.c_str()) : -1;

			if (itX != end && itY != end)
            { //dva niza
				if constexpr (!isComplex)
                {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, NameAndDataPtr[funD.xAxis], NameAndDataPtr[funD.yAxis], size, funD.xAxis, funD.yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, NameAndDataPtr[funD.xAxis], NameAndDataPtr[funD.yAxis], size, funD.xAxis, funD.yAxis));
				}
				else
                {

					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, NameAndDataPtr[xAxis], NameAndDataPtr[yAxis], size, funD.xAxis, funD.yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, NameAndDataPtr[xAxis], NameAndDataPtr[yAxis], size, funD.xAxis, funD.yAxis));
			
				}
				continue;
			}


			if (itX == end && paramIntX >= 0 && funD.yAxis.cCompare("0") == 0)
            { // parametar u x
				if constexpr (!isComplex)
                {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, solver->getParamsPtr() + paramIntX, nullptr, 1, funD.xAxis, ""));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntX])));
				}
				else
                {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntX])));
				}
				
				continue;
			}


			if (itY == end && paramIntY >= 0 && funD.xAxis.cCompare("0") == 0) { // parametar u y
        
				if constexpr (!isComplex)
                {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, solver->getParamsPtr() + paramIntY, nullptr, 1, funD.yAxis, ""));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntY])));
				}
				else
                {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, nullptr, 0, "", getCmplxString(solver->getParamsPtr()[paramIntY])));
				}
				
				continue;
			}


			if (itY != end && funD.xAxis.cCompare("0") == 0) { // jedan niz

				if constexpr (!isComplex)
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, NameAndDataPtr[funD.yAxis], size, "", funD.yAxis));
				else {
					outDataSet.push_back(DataDraw::FunctionDesc(funD.name, nullptr, NameAndDataPtr[yAxis], size, "", yAxis));
				}
				continue;
			}

			td::String log("Discarding function/dataset ");
			log += funD.name;
			logger->appendLog(log, LogType::Warning);


		}

		td::String nameAndType(modelName);
		nameAndType += "-graph";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outFuncs, DataDraw::Type::Graph);


		nameAndType = modelName;
		nameAndType += "-table";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outDataSet, DataDraw::Type::Table);

	
	}
};



enum class EquationTypes { NR, DAE, ODE, WLS };

int MainWindow::simulate(ViewForTab *tab)
{	
	const auto& logView = tab->getLog();
	bool error;
	const ModelNode &model = tab->getModelNode(error);
	if(error){
		logView->appendLog(tr("simulationFailed"), LogType::Error);
		return -1;
	}
	
	bool isComplex;
	if(auto it = model._attribs.find("domain"); it == model._attribs.end())
		isComplex = false;
	else{
		isComplex = (it->second.cCompareNoCase("cmplx") == 0 || it->second.cCompareNoCase("complex") == 0) ? true : false;
		if(!isComplex && it->second.cCompareNoCase("real") != 0)
			logView->appendLog("model attribute 'domain' is not 'real' or 'complex', assuming real", LogType::Warning);
	}


    const char* testFileName = nullptr;
    
	EquationTypes equationType;
	if(auto it = model._attribs.find("type"); it == model._attribs.end()){
		logView->appendLog("model does not have attribute 'type', assuming 'NR'(nonlinear equations)", LogType::Warning);
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
			logView->appendLog(msg, LogType::Error);
			logView->appendLog("Supported types are: NR, DAE, ODE, WLS", LogType::Info);
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
#else
	td::String mPath = "./modelForSimulator.xml";
	model.printNode(mPath);
#endif
#endif

	//mPath = "/home/bots/Desktop/model-Solver/real/ACGenWith1Load_WithLimit_Comp_DAE_RK4.xml";
	//mPath = "/home/bots/Desktop/model-Solver/real/PF_WLimits.xml";

	auto initSimulation = [&](auto pSolver)
    {
		td::String err;
		bool initSucess = false;
		if constexpr (std::is_same<decltype(pSolver), sc::IDblSolver*>::value)
        {
			switch (equationType) {
			case EquationTypes::NR:
                pSolver = sc::createDblNRSolver(maxIter);
				break;
			case EquationTypes::ODE:
				pSolver = sc::createDblODESolver();
				break;
			case EquationTypes::DAE:
				pSolver = sc::createDblDAESolver(maxIter);
				break;
			case EquationTypes::WLS:
                pSolver = sc::createDblWLSSolver(maxIter);
				break;
			}
            initSucess = pSolver->init(modelStr, sc::IDblSolver::SourceType::Memory);
			//initSucess = s->init(mPath, sc::IDblSolver::SourceType::File);
		}
		else if constexpr (std::is_same<decltype(pSolver), sc::ICmplxSolver*>::value)
        {
			switch (equationType){
            case EquationTypes::NR:
                pSolver = sc::createCmplxNRSolver(maxIter);
				break;
			case EquationTypes::ODE:
                    logView->appendLog("Complex ODE solver will be implemented in the future", LogType::Error);
                    return -1;
					//s = sc::createCmplxODESolver();
				break;
			case EquationTypes::DAE:
				//s = sc::createCmplxDAESolver(maxIter);
                logView->appendLog("Complex DAE solver will be implemented in the future", LogType::Error);
                return -1;
			case EquationTypes::WLS:
                pSolver = sc::createCmplxWLSSolver(maxIter);
				break;
			}
			initSucess = pSolver->init(modelStr, sc::ICmplxSolver::SourceType::Memory);
		}
	else{
		static_assert(pSolver==nullptr, "wrong pointer type for initSimulation");
		return 0;
	}


		bool useAutoFuncs = false;
		std::vector<ModelSettings::FunctionDesc> autoFuncs;
		int size = (equationType == EquationTypes::NR || equationType == EquationTypes::WLS) ? 1 : 1 + std::abs(startTime - endTime) / stepTime;

		if (funcs.empty() && initSucess) {
			useAutoFuncs = true;
			cnt::SafeFullVector<td::INT4> symbs;

			pSolver->getOutSymbols(symbs);
			int timeIndex = pSolver->getParamIndex("t");
			
			if (timeIndex < 0) 
				for (const auto& symIndex : symbs)
					autoFuncs.push_back(ModelSettings::FunctionDesc(ModelSettings::FunctionDesc::Type::points, pSolver->getSymbolName(symIndex), pSolver->getSymbolName(symIndex), "0"));
			else 
				for (const auto& symIndex : symbs)
					autoFuncs.push_back(ModelSettings::FunctionDesc(ModelSettings::FunctionDesc::Type::graph, pSolver->getSymbolName(symIndex), pSolver->getSymbolName(symIndex), "t"));

			if (autoFuncs.empty())
				logView->appendLog("No out variables found. You must add 'out=true' attribute to a single variable or the variable declaration tag for them to be visible to the plotter", LogType::Warning);
		}
        auto pResults = tab->getResults();
        td::Timer timer;
        timer.start();
		SolutionBuffer buffer(tab->getName(), logView, pResults, useAutoFuncs ? autoFuncs : funcs, pSolver, initSucess);
        auto initDuration = timer.getDurationInSeconds();
        td::String strTmp;
        strTmp.format("Preparation time:  %.3lf seconds", initDuration);
        logView->appendLog(strTmp, LogType::Info);

		if(initSucess)
        {
			if(equationType == EquationTypes::DAE || equationType == EquationTypes::ODE)
            {
                pSolver->setOutFilter(sc::OutFilter::InitialValues);
                pSolver->solve(startTime, stepTime, endTime, &buffer, 0);
            }
			if(equationType == EquationTypes::NR || equationType == EquationTypes::WLS)
            {
				pSolver->solve();
				buffer.put();
				err = pSolver->getLastErrorStr();
				buffer.finalize(err.isNull() ? sc::ISolutionBuffer::Status::SuccessfullyCompleted : sc::ISolutionBuffer::Status::Error);
			}
			return 0;
		}

		buffer.finalize(sc::ISolutionBuffer::Status::Error);

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

