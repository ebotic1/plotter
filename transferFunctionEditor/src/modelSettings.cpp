#include "modelSettings.h"
#include "globals.h"


modelSettings::modelSettings() : v(4), name("model name", td::DataType::string8, "name of the model that will be generated"), editLbl("Paramaters:"), exportBtn("Export to XML") {

	edit.setSize(gui::Size(100,800));

	v.append(name);
	globals::model_settings = this;

	v  << editLbl << edit;
	edit.hide(true, false);

	setLayout(&v);

	
}

