#include <gui/View.h>
#include <gui/TextEdit.h>
#include <gui/HorizontalLayout.h>


class CodeEdit : public gui::TextEdit {

public:

	CodeEdit();
	bool onKeyPressed(const gui::Key& key) override;
	bool onKeyReleased(const gui::Key& key) override;
	void processText(const gui::Range& r);

};


