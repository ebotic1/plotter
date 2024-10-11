#pragma once
#include "codeEdit.h"
#include "../../../EquationToXML/inc/nodes.h"
#include <gui/HorizontalLayout.h>
#include "../baseView.h"



class TextEditorView : public ViewForTab::BaseClass {

	CodeEdit textMain;
	gui::HorizontalLayout _hl;

	friend class CodeEdit;
public:

	

	TextEditorView();
	void getModel(modelNode& model) override;
	bool save(const td::String& path, const td::String& settingsString) override;
	void saveAs(const td::String& settingsString, td::String *newPath) override;
	bool openFile(const td::String& path, td::String& settingsString) override;

};