#include "modelSettings.h"
#include "globals.h"
#include "modelSettings.h"


modelSettings::modelSettings() : v(3), name("model name", td::DataType::string8, "name of the model that will be generated"), editLbl("Paramaters:"), exportBtn("Export to XML") {

	v.append(name);
	globals::model_settings = this;

	v  << editLbl << edit;

	setLayout(&v);

	
}
