#include "modelSettings.h"
#include "globals.h"

modelSettings::modelSettings() : v(1), name("model name", td::DataType::string8, "name of the model that will be generated") {

	v.append(name);
	setLayout(&v);
	globals::model_settings = this;

}
