#include "mainWindow.h"
#include "settingsDialog.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"
#include <xml/DOMParser.h>

std::vector<fileModels> MainWindow::loadedModels;

void MainWindow::simulate()
{
    if(_tabView.getNumberOfViews() == 0)
        return;

    simulate((ViewForTab*)_tabView.getCurrentView());
}

MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1100, 800)),
      _tabView(gui::TabHeader::Type::Dynamic, 5, 25),
      textEditorIcon(":txtIcon"),
      guiEditorIcon(":guiIcon"),
      _switcherView(2)
{
    setTitle("Model Maker");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);
    setContextMenus(&_contextMenu);

    _switcherView.addView(&startingView, true);
    setCentralView(&_switcherView);


    _tabView.onClosedView([this](int) {
        if (_tabView.getNumberOfViews() == 1)
            showStartScreen(true);
    });

    _switcherView.addView(&_tabView);
    
}

void MainWindow::showStartScreen(bool show)
{
    if(show){
        if(_switcherView.getCurrentViewPos() == 0)
            return;
        //_mainMenuBar.enableModelMenu(false);
        _switcherView.showView(0);

    }else{
        if (_switcherView.getCurrentViewPos() == 1)
            return;
        //_mainMenuBar.enableModelMenu(true);
        _switcherView.showView(1);
    }
}

void MainWindow::changeTabName(const td::String &name, ViewForTab *tab)
{
    tab->setName(name);
    for(int i = 0; i<_tabView.getNumberOfViews(); ++i)
        if(_tabView.getView(i) == tab){
            //_tabView.changeTabName(i, name);
            return;
        }

}

bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{
    int menuID = aiDesc._menuID;

    if(menuID == toolBarID){ //ako je izvor toolbar

        auto action = toolBarActionIDs(aiDesc._actionItemID);

        if(action == toolBarActionIDs::Simulate){
            simulate();
            return true;
        }

        return false;
    }

    //izvor mora biti menu bar ili poziv preko global events
    menuBarActionIDs action = (menuBarActionIDs) aiDesc._actionItemID;
    subMenuIDs subMenuID = (subMenuIDs) aiDesc._firstSubmenuID;

    if(action == menuBarActionIDs::Settings){
        auto settings = new settingsDialog(this);
        settings->setResizable(false);
        settings->keepOnTopOfParent();
        settings->open();
        return true;
    }
    

    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewGraphical){
        showStartScreen(false);
        _tabView.addView(new ViewForTab(new GraphicalEditorView), tr("newGraphTab"), &guiEditorIcon);
        auto ptr = ((ViewForTab *)(_tabView.getView(_tabView.getNumberOfViews() - 1)));
        ptr->setName(tr("newGraphTab"));
        const_cast<ViewForTab::BaseClass&>(ptr->getMainView()).setFocus(true);
        return true;
    }


    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewText){
        showStartScreen(false);
        _tabView.addView(new ViewForTab(new TextEditorView), tr("newTextTab"), &textEditorIcon);
        auto ptr = ((ViewForTab *)(_tabView.getView(_tabView.getNumberOfViews() - 1)));
        ptr->setName(tr("newTextTab"));
        const_cast<ViewForTab::BaseClass&>(ptr->getMainView()).setFocus(true);
        return true;
    }

    ViewForTab *currentView = (ViewForTab*)this->_tabView.getCurrentView();
    if(_tabView.getNumberOfViews() == 0)
        return true;

    if(action == menuBarActionIDs::Simulate){
        simulate(currentView);
        return true;
    }

    if(action == menuBarActionIDs::OpenFromFile && subMenuID == subMenuIDs::subMenuNewModel){

    }

    if(action == menuBarActionIDs::Export){
        auto path = new gui::SaveFileDialog(this, tr("ExporttoXML"), ".xml", tr("Export"), currentView->getName().c_str());
        path->openModal([currentView](gui::FileDialog* pDlg) {
            td::String path(pDlg->getFileName());
            if(!path.isNull()){
                currentView->exportToXML(path);
            }
        });
        return true;
    }

  
    return false;
}


const modelNode &MainWindow::getModelFromTabOrFile(const td::String &modelNameOrPath)
{
    bool sucess;
    if(modelNameOrPath.endsWith(".xml")){//file
        if(!std::filesystem::exists(modelNameOrPath.c_str()))
            throw (exceptionCantAccessFile) modelNameOrPath;
        
        auto time = std::filesystem::last_write_time(modelNameOrPath.c_str()).time_since_epoch();
        for(auto &m: loadedModels){
            if(m.path == modelNameOrPath)
                if(time >= m.timeModified)
                    return m.model;
                else{
                    sucess = m.model.readFromFile(modelNameOrPath);
                    if(!sucess)
                        throw (exceptionCantAccessFile) modelNameOrPath;
                    m.timeModified = time;
                    return m.model;
                }
        }    

        loadedModels.emplace_back(modelNameOrPath);
        sucess = loadedModels.back().model.readFromFile(modelNameOrPath);
        if(!sucess)
            throw (exceptionCantAccessFile) modelNameOrPath;
        loadedModels.back().timeModified = time;
        return loadedModels.back().model;
        
	
    }else{ //tab

        for(int i = 0; i<_tabView.getNumberOfViews(); ++i)
            if(((ViewForTab *)_tabView.getView(i))->getName() == modelNameOrPath){
                sucess = true;
                return ((ViewForTab *)_tabView.getView(i))->getModelNode();
            }        
        throw (exceptionCantFindTab) modelNameOrPath;
    }

    static modelNode m; // should never run
    return m;
}



MainWindow::~MainWindow(){}

