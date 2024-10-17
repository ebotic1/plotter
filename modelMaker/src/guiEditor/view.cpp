#include "view.h"
#include "globals.h"
#include <gui/FileDialog.h>


kanvas* ptr;

GraphicalEditorView::GraphicalEditorView() : spliter(gui::SplitterLayout::Orientation::Horizontal){
	ptr = &_canvas;
	spliter.setContent(_canvas, props);
	setLayout(&spliter);
}

bool GraphicalEditorView::save(const td::String& path, const td::String& settingsString)
{
	return _canvas.saveState(path, settingsString);
}

void GraphicalEditorView::saveAs(const td::String& settingsString, td::String* newPath)
{
	gui::SaveFileDialog& saveDiag = *new gui::SaveFileDialog(this, tr("saveTF"), { {tr("graphicalEditorSaveExpl"), "*.tfstate"}});
	saveDiag.openModal([this, newPath, &settingsString](gui::FileDialog *d) {
		if (td::String path = d->getFileName(); !path.isNull()) {
			if(_canvas.saveState(path, settingsString))
				*newPath = path;
		}
	});

}

void GraphicalEditorView::getModel(modelNode& model)
{
	_canvas.getModel(model);
}

void GraphicalEditorView::refreshVisuals()
{
	globals::refreshCanvas();
}

bool GraphicalEditorView::openFile(const td::String& path, td::String& settingsString)
{
	return _canvas.restoreState(path, settingsString);
}


void globals::refreshCanvas() {
	ptr->reDraw();
}

