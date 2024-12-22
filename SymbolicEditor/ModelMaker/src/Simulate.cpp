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
	const std::vector<ModelSettings::FunctionDesc> &functions;
	const std::shared_ptr<LogView> logger;
    SolverTemplate<T> *solver;
    Results* _pResults;
    dp::IDataSet* _pDS;
	std::chrono::steady_clock::time_point startTime;
	int cnt = 0, maxPoints;
    T* _pSymbolValues = nullptr;
//    T* _pParams = nullptr;
    cnt::SafeFullVector<td::INT4> _outSymbols;
//    const td::INT4* _pOutParams = nullptr;
//    fo::OutFile _f;
    double _dTRep=1;
    td::UINT4 _nVars = 0;
    td::UINT4 _nOutSymbols = 0;
    int _indexOfTimeSymbol = -1;

	std::vector<std::pair<double*, T*>> storePointers, storePointersImag;
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
    SolutionBuffer(const td::String &modelName, const std::shared_ptr<LogView> logger, bool initSucess, const std::vector<ModelSettings::FunctionDesc> &functions, int maxPoints, SolverTemplate<T>* solver, Results* pResults)
    : functions(functions)
    , logger(logger)
    , solver(solver)
    , maxPoints(maxPoints)
    , modelName(modelName)
    , startTime(std::chrono::steady_clock::now())
    , _pResults(pResults)
    {
        
        //IDz: Ovaj dio je bespotrebno kompleksan!!
        //IDz: Prijedlozi 1. rezultati se upisuju samo u Results tabelu
        //                2. Prostsolution komande treba koristiti za crtanje grafova iz Results tabele
        //                3. Exportovanje rezultata u XML i CSV se jednostavno može dodati u Results tabelu
        //                4. Izbaciti sve mape i komplikacije
        //IDz: Time bi se popunjavanjavnje u ovom dijelu svelo na kod koji popunjava Results tabelu i imala bi se jednostavna sekvenca
        
        if(!initSucess)
            return;
        
        _nVars = solver->getNumberVariables();
        _pSymbolValues = solver->getSymbolValuesPtr();
        
        _nOutSymbols = (td::UINT4) solver->getNoOfOutSymbols();
        
        solver->getOutSymbols(_outSymbols);
//        _pParams = solver->getParamsPtr();
        
        
        constexpr bool isComplex = std::is_same<T, td::cmplx>::value;
//        cnt::SafeFullVector<td::INT4> outSymbols;
//        solver->getOutSymbols(outSymbols);
        
        std::map<td::String, td::INT4> outVars;
        
        for (const auto& index : _outSymbols)
            outVars[solver->getSymbolName(index)] = index;
        
        size_t bindColumns = _nOutSymbols;
        int indexOfTimeParameter = solver->getParamIndex("t");
        if (indexOfTimeParameter >= 0)
        {
            _indexOfTimeSymbol = indexOfTimeParameter + _nVars;
            outVars["t"] = indexOfTimeParameter;
            ++bindColumns;
        }
	
        auto nOutVars = outVars.size();
        _pDS = _pResults->getDataSet();
        bool columnsChanged = false;  //IDz: Za Emira --> graf nema iste varijable kao ranije ako je ovo true
        if (_pDS)
        {
            //check column names of out variables wih the current column names
            size_t nCols = _pDS->getNumberOfCols();
            if (nCols != bindColumns)
            {
                _pDS = nullptr;
                columnsChanged = true;
            }
            else
            {
                int iPos = 0;
                if (_indexOfTimeSymbol > 0)
                {
                    const td::String& firstColumnName = _pDS->getColName(0);
                    if (firstColumnName.compare("t", 1))
                        iPos = 1;
                    else
                    {
                        _pDS = nullptr;
                        columnsChanged = true;
                    }
                }
                
                if (_pDS)
                {
                    //check other columns
                    for (auto iSmb : _outSymbols)
                    {
                        const td::String& columnName = _pDS->getColName(iPos++);
                        const char* outColName = solver->getSymbolName(iSmb);
                        if (columnName.cCompare(outColName) != 0)
                        {
                            _pDS = nullptr;
                            columnsChanged = true;
                            break;
                        }
                    }
                }
            }
        }
        else
            columnsChanged = true;
                         
        if (!_pDS)
        {
            //create DataSet
            _pDS = dp::createConnectionlessDataSet(dp::IDataSet::Size::Medium);
            
            dp::DSColumns cols(_pDS->allocBindColumns(bindColumns));
            
            if (_indexOfTimeSymbol > 0)
                cols << "t" << td::real8;
            
            for (auto iSmb : _outSymbols)
            {
                auto sn = solver->getSymbolName(iSmb);
                cols << sn << td::real8;
            }
            
            _pDS->execute();
        }
        else
        {
            _pDS->removeAll();
        }
        
        //IDz: OD OVE POZICIJE IZBACITI KOD
		std::map<td::String, td::INT4>::iterator itX, itY, end = outVars.end();
		td::String xAxis, yAxis;
		int paramIntX, paramIntY;

		for (const auto& funD : functions)
        {
			if (funD.xAxis.cCompare("0") == 0 && funD.yAxis.cCompare("0") == 0)
				continue;


#ifdef MU_DEBUG
			td::String log2("Processing function: ");
			log2 += funD.name;
			logger->appendLog(log2, LogType::Warning);
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

			if (itX != end && itY != end)
            { //dva niza
				if constexpr (!isComplex)
                {
					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(funD.xAxis), getPointerToVar(funD.yAxis), maxPoints, funD.xAxis, funD.yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(funD.xAxis), getPointerToVar(funD.yAxis), maxPoints, funD.xAxis, funD.yAxis));
				}
				else
                {

					if (funD.type == ModelSettings::FunctionDesc::Type::graph)
						outFuncs.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(xAxis), getPointerToVar(yAxis), maxPoints, xAxis, yAxis));
					else
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, getPointerToVar(xAxis), getPointerToVar(yAxis), maxPoints, xAxis, yAxis));
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
						outDataSet.push_back(DataDraw::FunctionDesc(funD.name, solver->getParamsPtr() + paramIntX, nullptr, 1, funD.xAxis, ""));
				}
				else
                {
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
			logger->appendLog(log, LogType::Warning);
		}

        storePointers.reserve(nOutVars);
        
		td::String nameWithoutExtension;
		for (const auto& nameAndPtr : NameToStorePtrCont) {
			if (nameAndPtr.first.cCompare("t") == 0) {
				storePointers.push_back({ nameAndPtr.second, solver->getParamsPtr() + outVars[nameAndPtr.first]});
				continue;
			}

			if constexpr (!isComplex)
				storePointers.push_back({ nameAndPtr.second, solver->getVariablesPtr() + outVars[nameAndPtr.first]});
			else
            {
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
        
		if constexpr (std::is_same<T, td::cmplx>::value) {
			for (int i = 0; i < storePointers.size(); ++i)
				storePointers[i].first[cnt] = (storePointers[i].second)->real();
			for (int i = 0; i < storePointersImag.size(); ++i)
				storePointersImag[i].first[cnt] = (storePointers[i].second)->imag();
		}
		else
        {
            auto nOutVars = storePointers.size();
            auto& row = _pDS->getEmptyRow();
            size_t iPos = 0;
            if (_indexOfTimeSymbol > 0)
            {
                row[0] = _pSymbolValues[_indexOfTimeSymbol];
                iPos = 1;
            }
            for (auto i : _outSymbols)
                row[iPos++] = _pSymbolValues[i];
            _pDS->push_back();
            
            //IDz: Izbaciti
            for (int i = 0; i < nOutVars; ++i)
            {
                double val = *storePointers[i].second;
                storePointers[i].first[cnt] = val;
            }
		}

		++cnt;
	}

	void finalize(Status status) override
	{
        _pResults->show(_pDS);
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
			str.getString(log);
			logger->appendLog(log, LogType::Info);
		}

		td::String nameAndType(modelName);
		nameAndType += "-graph";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outFuncs, DataDraw::Type::Graph);

#ifdef USE_OLD_TABLES
		nameAndType = modelName;
		nameAndType += "-table";
		GlobalEvents::getMainWindowPtr()->getDataDrawer()->addData(nameAndType, outDataSet, DataDraw::Type::table);
#endif
	
	}
};

//IDz: Bespotrebno kompliciranje!!
//SolutionBuffer(const td::String &, const std::shared_ptr<LogView>, bool, const std::vector<ModelSettings::FunctionDesc> &, int, sc::IDblSolver *, Results*) -> SolutionBuffer<sc::IDblSolver>;
//SolutionBuffer(const td::String &, const std::shared_ptr<LogView>, bool, const std::vector<ModelSettings::FunctionDesc> &, int, sc::ICmplxSolver *, Results*) -> SolutionBuffer<sc::ICmplxSolver>;



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
        Results* pResults = tab->getResults();
        td::Timer timer;
        timer.start();
        SolutionBuffer buffer(tab->getName(), logView, initSucess, useAutoFuncs ? autoFuncs : funcs, size, pSolver, pResults);
        auto initDuration = timer.getDurationInSeconds();
        td::String strTmp;
        strTmp.format("Preparation time:  %.3lf seconds", initDuration);
        logView->appendLog(strTmp, LogType::Info);

		if(initSucess)
        {
            //TODO: Dodati nacin kako upisati rezultate u Results
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

