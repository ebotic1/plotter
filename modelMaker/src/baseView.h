#pragma once
#include <gui/View.h>

class BaseViewForTab : public gui::View {

public:


	virtual void simulate(double startTime, double endTime, double stepTime){

	}

	virtual bool loadFile(const td::String& path) {
		return false;
	}
	virtual bool save() {
		return false;
	}
	virtual bool saveAs(td::String path) {
		return false;
	}
	virtual bool exportToXML(td::String path) {
		return false;
	}

};