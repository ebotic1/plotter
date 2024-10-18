#include "mainWindow.h"
#include <sc/SCLib.h>
#include <sc/ISolutionBuffer.h>
#include <type_traits>
#include <chrono>
#include "textEditor/View.h"
#include "baseView.h"
#include "modelSettings.h"
#include "dataDrawer.h"


using LogType = LogView::LogType;


template<typename solverType, typename resultType>
class SolutionBuffer : public sc::ISolutionBuffer {
	const std::vector<ModelSettings::FunctionDesc> &functions;
	const LogView *logger;
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
	SolutionBuffer(const td::String &modelName, const LogView *logger, const std::vector<ModelSettings::FunctionDesc> &functions, int maxPoints, solverType *solver):
		functions(functions),
		logger(logger),
		solver(solver),
		maxPoints(maxPoints),
		modelName(modelName),
		startTime(std::chrono::steady_clock::now())
	{
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




			if (itX == end && paramIntX > 0 && funD.yAxis.cCompare("0") == 0) { // parametar u x
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


			if (itY == end && paramIntY > 0 && funD.xAxis.cCompare("0") == 0) { // parametar u y
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
			for (int i = 0; i < storePointers.size(); ++i)
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


SolutionBuffer(const td::String &, const LogView *, const std::vector<ModelSettings::FunctionDesc> &, int, sc::IDblSolver *) -> SolutionBuffer<sc::IDblSolver, double>;
SolutionBuffer(const td::String &, const LogView *, const std::vector<ModelSettings::FunctionDesc> &, int, sc::ICmplxSolver *) -> SolutionBuffer<sc::ICmplxSolver, td::cmplx>;


enum class EquationTypes { NR, DAE, ODE, WLS };



void MainWindow::simulate(ViewForTab *tab)
{
	const auto& logView = tab->getLog();
	bool error;
	const modelNode &model = tab->getModelNode(error);
	if(error){
		logView.appendLog(tr("simulationFailed"), LogType::error);
		return;
	}
	
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

	
#ifdef MU_DEBUG
	model.printNode("./modelForSimulator.xml"); //u slucaju problema da se moze pogledati finalni generisani model koji je poslan solveru
#endif



	auto initSimulation = [&](auto s) {
		td::String err;
		if constexpr (std::is_same<decltype(s), sc::IDblSolver*>::value) {
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
			s->init(modelStr, sc::IDblSolver::SourceType::Memory);
		}
		else if constexpr (std::is_same<decltype(s), sc::ICmplxSolver*>::value) {
			switch (equationType) {
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
		}
	else
		static_assert(false, "wrong pointer type for initSimulation");

		bool useAutoFuncs = false;
		std::vector<ModelSettings::FunctionDesc> autoFuncs;
		int size = (equationType == EquationTypes::NR) ? 1 : 1 + std::abs(startTime - endTime) / stepTime;

		if (funcs.empty()) {
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
			
		}

		auto buffer = new SolutionBuffer(tab->getName(), &logView, useAutoFuncs ? autoFuncs : funcs, size, s);
		s->solve(startTime, stepTime, endTime, buffer, 0);
		err = s->getLastErrorStr();


		if (equationType == EquationTypes::NR) {
			buffer->put();
			buffer->finalize(err.isNull() ? sc::ISolutionBuffer::Status::SuccessfullyCompleted : sc::ISolutionBuffer::Status::Error);
		}


	};





	if (isComplex) {
		sc::ICmplxSolver* s = nullptr;
		initSimulation(s);
	}
	else {
		sc::IDblSolver* s = nullptr; 
		initSimulation(s);
	}
		
		
	


}

