#pragma once
#include <gui/MenuBar.h>
#include <gui/MenuItem.h>

enum menuBarActionIDs{New=1, Save, SaveAs, Open, Export, Settings, EmptyModel};


enum subMenuIDs{subMenuModel=1, subMenuNewModel, subMenuSettings, subMenuNewText, subMenuNewGraphical};


class MenuBar : public gui::MenuBar {
	gui::SubMenu model, newModel, settings;
	gui::SubMenu modelText, modelGraphical;
public:
	MenuBar() : gui::MenuBar(3), 
	model(subMenuModel, tr("Model"), 3),
	newModel(subMenuNewModel, tr("NewTab"), 2),
	settings(subMenuSettings, tr("settingsShort"), 1),
	modelText(subMenuNewText, tr("TextualEditor"), 1),
	modelGraphical(subMenuNewGraphical, tr("GraphicalEditor"), 1)
	{
		{
			auto &items = model.getItems();
			items[0].initAsActionItem(tr("Save"), Save, "<Ctrl>s");
			items[1].initAsActionItem(tr("SaveAs"), SaveAs, "<Ctrl><Alt>s");
			items[2].initAsActionItem(tr("ExporttoXML"), Export);
		}
		{
			auto &items = settings.getItems();
			items[0].initAsActionItem(tr("settingsShort"), Settings, tr("<Ctrl>,").c_str());
		}

		{
			auto &items = modelGraphical.getItems();
			items[0].initAsActionItem(tr("emptyModel"), EmptyModel, "<Ctrl><Alt>n");
		}


		{
			auto &items = modelText.getItems();
			items[0].initAsActionItem(tr("emptyModel"), EmptyModel, "<Ctrl>n");
		}

		{
			auto &items = newModel.getItems();
			items[0] = modelText;
			items[1] = modelGraphical;
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