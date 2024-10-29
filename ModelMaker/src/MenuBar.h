#pragma once
#include <gui/MenuBar.h>
#include <gui/MenuItem.h>

enum menuBarActionIDs{New=1, Simulate, Save, SaveAs, Export, Settings, EmptyModel, OpenFromFile, ODE, NR, WLS, DAE};

enum subMenuIDs{subMenuModel=1, subMenuNewModel, subMenuSettings, subMenuNewText, subMenuNewGraphical};


class MenuBar : public gui::MenuBar {
	gui::SubMenu model, newModel, settings;
	gui::SubMenu modelText, modelGraphical;
public:
	MenuBar() : gui::MenuBar(3), 
	model(subMenuModel, tr("Model"), 3),
	newModel(subMenuNewModel, tr("NewTab"), 3),
	settings(subMenuSettings, tr("app"), 2),
	modelText(subMenuNewText, tr("TextualEditor"), 5),
	modelGraphical(subMenuNewGraphical, tr("GraphicalEditor"), 1)
	{
		{
			auto &items = model.getItems();
			items[0].initAsActionItem(tr("Save"), Save, "<Ctrl>s");
			items[1].initAsActionItem(tr("SaveAs"), SaveAs, "<Ctrl><Alt>s");
			items[2].initAsActionItem(tr("ExporttoXML"), Export, "<Ctrl>e");
		}
		{
			auto &items = settings.getItems();
			items[0].initAsActionItem(tr("settingsShort"), Settings, tr("<Ctrl>,").c_str());
			items[1].initAsActionItem(tr("startToolTip"), Simulate, "<F5>");
		}

		{
			auto &items = modelGraphical.getItems();
			items[0].initAsActionItem(tr("emptyModel"), EmptyModel, "<Ctrl><Alt>n");
		}


		{
			auto &items = modelText.getItems();
			items[0].initAsActionItem(tr("emptyModel"), EmptyModel, "<Ctrl>n");
			items[1].initAsActionItem(tr("odeModel"), ODE);
			items[2].initAsActionItem(tr("nrModel"), NR);
			items[3].initAsActionItem(tr("wlsModel"), WLS);
			items[4].initAsActionItem(tr("daeModel"), DAE);

		}

		{
			auto &items = newModel.getItems();
			items[0].initAsActionItem(tr("openFromFile"), OpenFromFile, "<Ctrl>o");
			items[1] = modelText;
			items[2] = modelGraphical;
		}
		


		_menus[0] = &model;
		_menus[1] = &newModel;
		_menus[2] = &settings;
			
	}

	void enableModelMenu(bool enable){
		if(enable)
			model.enable();
		else
			model.disable();
	}

	~MenuBar(){}
};