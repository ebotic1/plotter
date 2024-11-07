#include "MainWindow.h"
#include "SettingsDialog.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"
#include <xml/DOMParser.h>
#include <tuple>

#include <gui/TableEdit.h>

std::vector<fileModels> MainWindow::_loadedModels;

void MainWindow::simulate()
{
    if(_tabView.getNumberOfViews() == 0)
        return;

    auto view = dynamic_cast<ViewForTab*>(_tabView.getCurrentView());
    if(view != nullptr)
        simulate(view);
}

MainWindow::MainWindow()
    : gui::Window(gui::Geometry(100, 10, 1500, 1000)),
      _tabView(gui::TabHeader::Type::Dynamic, 5, 25),
      _textEditorIcon(":txtIcon"),
      _guiEditorIcon(":guiIcon"),
      _switcherView(2),
      _dataDrawer(&_tabView)
{
    setTitle("Model Maker");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);
    setContextMenus(&_contextMenu);
    _switcherView.addView(&_startingView, true);


    GlobalEvents::settingsVars.loadSettingsVars(getApplication());
    if (GlobalEvents::settingsVars.embedPlot) {
        _plotEmbedded = true;
    }

    _tabView.onClosedView([this](int) {

        if (_tabView.getNumberOfViews() == 1)
            showStartScreen(true);
    });

    _switcherView.addView(&_tabView);
    setCentralView(&_switcherView);

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
            _tabView.setTitle(i, name);
            return;
        }


}

void MainWindow::closeTab(gui::BaseView *tab)
{
    for(int i = 0; i<_tabView.getNumberOfViews(); ++i)
        if(_tabView.getView(i) == tab){
            _tabView.removeView(i);
        }
    if(_tabView.getNumberOfViews() == 0)
        showStartScreen(true);
}

void MainWindow::addTab(ViewForTab::BaseClass *tab, const td::String &settingsStr, const td::String &path){
    ViewForTab *wholeTab = nullptr;
    _tabsToProcess.clear();
    if(GraphicalEditorView *ptr = dynamic_cast<GraphicalEditorView *>(tab); ptr != nullptr){
        wholeTab = new ViewForTab(ptr, settingsStr);
        _tabView.addView(wholeTab, "", &_guiEditorIcon);
        changeTabName(wholeTab->getName().isNull()  ? tr("newGraphTab") : wholeTab->getName(), wholeTab);
        wholeTab->setPath(path);
    }
    else if(TextEditorView *ptr = dynamic_cast<TextEditorView *>(tab); ptr != nullptr){
        wholeTab = new ViewForTab(ptr, settingsStr);
        _tabView.addView(wholeTab, "", &_textEditorIcon);
        changeTabName(wholeTab->getName().isNull()  ? tr("newTextTab") : wholeTab->getName(), wholeTab);
        if (path.endsWith(".modl"))
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

    static const auto removePath = [this](){
#ifndef MU_DEBUG
        ViewForTab *view = dynamic_cast<ViewForTab*>(_tabView.getView(_tabView.getNumberOfViews() - 1));
        if (view == nullptr)
            return;

        view->setPath("");
#endif // !MU_DEBUG
    };

    if(action == menuBarActionIDs::ODE){
        openFile(gui::getResFileName(":ODE"));
        removePath();
    }


    if(action == menuBarActionIDs::DAE){
        openFile(gui::getResFileName(":DAE"));
        removePath();
    }


    if(action == menuBarActionIDs::NR){
        openFile(gui::getResFileName(":NR"));
        removePath();
    }

    if(action == menuBarActionIDs::WLS){
        openFile(gui::getResFileName(":WLS"));
        removePath();
    }

    
    ViewForTab *currentView = dynamic_cast<ViewForTab*>(this->_tabView.getCurrentView());

    if(action == menuBarActionIDs::OpenFromFile){
        auto &o = *new gui::OpenFileDialog(this, tr("openModel"), {{"Text model editor", "*.modl"}, {"XML model", "*.xml"}, {tr("tfFile"), "*.tfstate"} }, tr("open"));
        if(menuID == subMenuIDs::subMenuModel){
           if(currentView == nullptr)
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


   if(currentView == nullptr) //ostale akcije nemaju smisla ako nema ni jedan tab otvoren
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

bool MainWindow::openFile(const td::String& path)
{
   const char *ok = path.c_str();
    const auto openF = [this, &path](auto ptr) {
        td::String settings;
        if (ptr->openFile(path, settings)) {
            addTab(ptr, settings, path);
            return true;
        }
        else
            delete ptr;
        return false;
    };

    if (path.endsWith(".modl") || path.endsWith(".xml")) {
        auto ptr = new TextEditorView;
        return openF(ptr);
    }

    if (path.endsWith(".tfstate")) {
        auto ptr = new GraphicalEditorView;
        return openF(ptr);
    }


    return false;

}

void MainWindow::onInitialAppearance()
{

    int argc;
    const char** argv;
    bool success = false;
    std::tie(argc, argv) = getApplication()->getMainArgs();
    for (int i = 1; i < argc; ++i) {
        if(openFile(argv[i]))
            success = true;
    }

    if(!success && GlobalEvents::settingsVars.restoreTabs){

        cnt::PushBackVector<td::String> paths;
        auto pathsString = getAppProperties()->getValue<td::String>("previousSessionTabPaths", "");
        //getAppProperties()->setValue<td::String>("previousSessionTabPaths", "");
        pathsString.split(":", paths);
        for(const auto &path: paths)
            openFile(path);

    }

}

DataDraw* MainWindow::getDataDrawer(bool openWindow)
{

    static auto dataDrawerPtr = &_dataDrawer;

    if(!openWindow || _plotEmbedded)
        return dataDrawerPtr;


    if (getAttachedWindow((td::UINT4)dialogIDs::dataDrawer) == nullptr){
        dataDrawerPtr = new DataDraw; //workaround, view koji se nalazi u gui::Window prestaje biti validan nakon njegovog gasenja
        auto ptr = new DataDrawerWindow(this, dataDrawerPtr);
        ptr->setTitle("Simulation results");
        ptr->open();
    }


    return dataDrawerPtr;
}


const modelNode &MainWindow::getModelFromTabOrFile(const td::String &modelNameOrPath)
{
    bool success;
    if(modelNameOrPath.endsWith(".xml")){//file
        std::filesystem::path file(modelNameOrPath.c_str());
        if(!std::filesystem::exists(file))
            throw exceptionCantAccessFile{modelNameOrPath};
        
        auto time = std::filesystem::last_write_time(file).time_since_epoch();
        for(auto &m: _loadedModels){
            if(m.path == modelNameOrPath)
            {
                if(time == m.timeModified)
                    return m.model;
                else
                {
                    success = m.model.readFromFile(modelNameOrPath);
                    if(!success)
                        throw exceptionCantAccessFile {modelNameOrPath};
                    m.timeModified = time;
                    return m.model;
                }
            }
        }

        _loadedModels.emplace_back(modelNameOrPath);
        success = _loadedModels.back().model.readFromFile(modelNameOrPath);
        if(!success)
            throw exceptionCantAccessFile {modelNameOrPath};
        _loadedModels.back().timeModified = time;
        return _loadedModels.back().model;
        
	
    }else{ //tab
        ViewForTab* tab;
        for (int i = 0; i < _tabView.getNumberOfViews(); ++i){
            tab = dynamic_cast<ViewForTab*>(_tabView.getView(i));
            if(tab != nullptr && tab->getName() == modelNameOrPath){
                success = true;
                return tab->getModelNode(success);
            }        
        }
        throw exceptionCantFindTab {modelNameOrPath};
    }

    static modelNode m; // should never run
    return m;
}


bool MainWindow::prepareForClose()
{
    decltype(_tabsToProcess)::iterator it;
    ViewForTab* tab;

    for (int i = _tabView.getNumberOfViews() - 1; i >= 0; --i) {
        it = _tabsToProcess.find(_tabView.getView(i));
        if (it == _tabsToProcess.end())
            continue;

        tab = (ViewForTab*)*it;

        _tabsToProcess.erase(it);
        _tabView.showView(i);
        if (tab->promptSaveIfNecessary(true))
            return false;

    }


    _closeWindow = true;
    close();
    return true;

}



bool MainWindow::shouldClose()
{
    if (_closeWindow)
        return true;

    if (!_tabsToProcess.empty()) {
        return prepareForClose();
    }

    ViewForTab* tab;
    cnt::StringBuilder pathBuilder;

    for (int i = 0; i < _tabView.getNumberOfViews(); ++i) {
        tab = dynamic_cast<ViewForTab*>(_tabView.getView(i));
        if(tab == nullptr)
            continue;

        if(GlobalEvents::settingsVars.restoreTabs && !tab->getPath().isNull())
            pathBuilder << tab->getPath() << ":";

        if (GlobalEvents::settingsVars.warnBeforeClose)
           _tabsToProcess.emplace(tab);

    }

    pathBuilder.getString(_paths);
    getAppProperties()->setValue<td::String>("previousSessionTabPaths", _paths);
    GlobalEvents::settingsVars.saveValues();


    if (GlobalEvents::settingsVars.warnBeforeClose) {
        return prepareForClose();
    }

    return true;
}

MainWindow::~MainWindow()
{

    
}

DataDrawerWindow::DataDrawerWindow(gui::Window *parent, DataDraw* mainView):
    gui::Window(gui::Size(500, 500), parent, (td::UINT4)MainWindow::dialogIDs::dataDrawer),
    storedView(mainView)
{
    setCentralView(storedView);
}

DataDrawerWindow::~DataDrawerWindow()
{
    delete storedView;
}
