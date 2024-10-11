#include "mainWindow.h"
#include "settingsDialog.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"
#include <xml/DOMParser.h>
#include <tuple>

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


    int argc;
    const char** argv;
    std::tie(argc, argv) = getApplication()->getMainArgs();
    for (int i = 1; i < argc; ++i) {
        openFile(argv[i]);
    }
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



void MainWindow::addTab(ViewForTab::BaseClass *tab, const td::String &settingsStr, const td::String &path){
    ViewForTab *wholeTab = nullptr;
    if(GraphicalEditorView *ptr = dynamic_cast<GraphicalEditorView *>(tab); ptr != nullptr){
        wholeTab = new ViewForTab(ptr, settingsStr);
        _tabView.addView(wholeTab, tr("newGraphTab"), &guiEditorIcon);
        wholeTab->setName(tr("newGraphTab"));
        wholeTab->setPath(path);
    }
    else if(TextEditorView *ptr = dynamic_cast<TextEditorView *>(tab); ptr != nullptr){
        wholeTab = new ViewForTab(ptr, settingsStr);
        _tabView.addView(wholeTab, tr("newTextTab"), &textEditorIcon);
        wholeTab->setName(tr("newGraphTab"));
        if (path.endsWith(".txt"))
            wholeTab->setPath(path);
    }




    if(wholeTab != nullptr){
        showStartScreen(false);
    }
    else
        throw "addTab method from MainWindow failed to recognize tab type";
}



bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{

    if(aiDesc._menuID == toolBarID){ //ako je izvor toolbar

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
    subMenuIDs menuID = (subMenuIDs) aiDesc._menuID;

    if(action == menuBarActionIDs::Settings){
        auto settings = new settingsDialog(this);
        settings->setResizable(false);
        settings->keepOnTopOfParent();
        settings->open();
        return true;
    }
    

    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewGraphical){
        showStartScreen(false);
        addTab(new GraphicalEditorView, "");
        return true;
    }


    if(action == menuBarActionIDs::EmptyModel && subMenuID == subMenuIDs::subMenuNewText){
        showStartScreen(false);
        addTab(new TextEditorView, "");
        return true;
    }









    ViewForTab *currentView = (ViewForTab*)this->_tabView.getCurrentView();
 








    if(action == menuBarActionIDs::OpenFromFile){
        auto &o = *new gui::OpenFileDialog(this, tr("openModel"), {{"Text model", "*.txt"}, {"XML model", "*.xml"} }, tr("open"));
        if(menuID == subMenuIDs::subMenuModel){
           if(_tabView.getNumberOfViews() == 0)
                return true;
            o.openModal([currentView](gui::FileDialog* d){
                auto path = d->getFileName();
                if(path.isNull())
                    return;
                currentView->loadFile(path);
            });
        }
        if(menuID == subMenuIDs::subMenuNewModel){
                 o.openModal([this](gui::FileDialog* d){
            auto path = d->getFileName();
            if(path.isNull())
                return;
            
            openFile(path);

        });   
            return true;
        }
    }


   if(_tabView.getNumberOfViews() == 0) //ostale akcije nemaju smisla ako nema ni jedan tab otvoren
        return true;



    if(action == menuBarActionIDs::Simulate){
        simulate(currentView);
        return true;
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


    if(action == menuBarActionIDs::Save){
        currentView->save();
        return true;
    }

    if(action == menuBarActionIDs::SaveAs){
        currentView->saveAs();
        return true;
    }

  
    return false;
}

void MainWindow::openFile(const td::String& path)
{
    td::String settings;

    if (path.endsWith(".txt") || path.endsWith(".xml")) {
        auto ptr = new TextEditorView;
        if (ptr->openFile(path, settings)) {
            addTab(ptr, settings, path);
            return;
        }
        else
            delete ptr;
    }
}

const modelNode &MainWindow::getModelFromTabOrFile(const td::String &modelNameOrPath)
{
    bool sucess;
    if(modelNameOrPath.endsWith(".xml")){//file
        std::filesystem::path file(modelNameOrPath.c_str());
        if(!std::filesystem::exists(file))
            throw (exceptionCantAccessFile) modelNameOrPath;
        
        auto time = std::filesystem::last_write_time(file).time_since_epoch();
        for(auto &m: loadedModels){
            if(m.path == modelNameOrPath)
                if(time == m.timeModified)
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
                return ((ViewForTab *)_tabView.getView(i))->getModelNode(sucess);
            }        
        throw (exceptionCantFindTab) modelNameOrPath;
    }

    static modelNode m; // should never run
    return m;
}



MainWindow::~MainWindow(){}

