#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/GridLayout.h>
#include <vector>
#include <gui/Label.h>
#include <gui/NumericEdit.h>

class ModelSettings : public gui::View {
	gui::VerticalLayout _vl;
	gui::GridLayout _gridLayout;
	gui::Label _lblStart, _lblEnd, _lblStep, _lblMaxIter, _lblPreproc;
	gui::TextEdit preprocesCommands;
	gui::NumericEdit startTime, endTime, stepTime, maxIter;

	gui::View paramaterView;

public:

	struct FunctionDesc {
		enum class Type{graph, points} type;
		td::String name, xAxis, yAxis;
	};
	struct DependencyDesc {
		td::String pathOrTabName, alias;
	};

public:

	ModelSettings();
	void getDependencies(std::vector<DependencyDesc> &);
	void getFunctions(std::vector<FunctionDesc>&);
	void getStartStopTime(double& startTime, double& endTime, double& stepTime, unsigned int& maxIterations);

};