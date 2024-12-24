#include "MainWindow.h"
#include "SettingsView.h"
#include <gui/FileDialog.h>
#include "guiEditor/view.h"
#include "textEditor/View.h"
#include <xml/DOMParser.h>
#include <tuple>
#include "SimulatorView.h"

#include <gui/TableEdit.h>
#include "SaveDlg.h"
#include <gui/Panel.h>

std::vector<FileModels> MainWindow::_loadedModels;

void MainWindow::simulate()
{
    if(_tabView.getNumberOfViews() == 0)
        return;

    auto view = dynamic_cast<ViewForTab*>(_tabView.getCurrentView());
    if(view != nullptr)
        simulate(view);
}

MainWindow::MainWindow()
    : gui::Window(gui::Size(1500, 1000)),
      _tabView(gui::TabHeader::Type::Dynamic, 5, 25),
      _textEditorIcon(":txtIcon"),
      _guiEditorIcon(":guiIcon"),
      //_switcherView(2),
      _dataDrawer(&_tabView)
{
    setTitle("Model Maker");
    _mainMenuBar.setAsMain(this);
    setToolBar(_toolBar);
    setContextMenus(&_contextMenu);
    //_switcherView.addView(&_startingView, true);
    _tabView.setBackgroundView(&_startingView);
    _tabView.forwardMessagesTo(this);
    
    GlobalEvents::settingsVars.loadSettingsVars(getApplication());
    if (GlobalEvents::settingsVars.embedPlot) {
        _plotEmbedded = true;
    }

    _tabView.onClosedView([this](int) {
        //IDz: add toolbar updates
        //if (_tabView.getNumberOfViews() == 1)
          //  showStartScreen(true);
    });

    //_switcherView.addView(&_tabView);
    setCentralView(&_tabView);

}
//
//void MainWindow::showStartScreen(bool show)
//{
//    if(show){
//        if(_switcherView.getCurrentViewPos() == 0)
//            return;
//        //_mainMenuBar.enableModelMenu(false);
//        _switcherView.showView(0);
//
//    }else{
//        if (_switcherView.getCurrentViewPos() == 1)
//            return;
//        //_mainMenuBar.enableModelMenu(true);
//        _switcherView.showView(1);
//    }
//}

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
//    if(_tabView.getNumberOfViews() == 0)
//        showStartScreen(true);
}

template<typename tabType>
void MainWindow::addTab(const td::String &path){
    
    static_assert(std::is_base_of_v<ViewForTab::BaseClass, tabType>, "tabType must derive from ViewForTab::BaseClass");

    if constexpr (std::is_same_v<tabType, GraphicalEditorView>) {
        auto ptr = new SimulatorView<GraphicalEditorView>(tr("newGraphTab"), path);
        _tabView.addView(ptr, ptr->getName(), &_guiEditorIcon, (td::BYTE) DocumentType::ModelTFEditor);
    } else if constexpr (std::is_same_v<tabType, TextEditorView>) {
        auto ptr = new SimulatorView<TextEditorView>(tr("newTextTab"), path);
        _tabView.addView(ptr, ptr->getName(), &_textEditorIcon, (td::BYTE) DocumentType::ModelTxtEditor);
    } else {
        static_assert(false, "addTab() tabType not recognized");
    }
        
}

void MainWindow::removePath()
{
#ifndef MU_DEBUG
    ViewForTab *view = dynamic_cast<ViewForTab*>(_tabView.getView(_tabView.getNumberOfViews() - 1));
    if (view == nullptr)
        return;

    view->setPath("");
#endif // !MU_DEBUG
}

bool MainWindow::onActionItem(gui::ActionItemDescriptor& aiDesc)
{

//    if(aiDesc._menuID == toolBarID){ //ako je izvor toolbar
//
//        auto action = toolBarActionIDs(aiDesc._actionItemID);
//
//        if(action == toolBarActionIDs::Simulate){
//            simulate();
//            return true;
//        }
//
//        return false;
//    }

    //izvor mora biti menu bar ili poziv preko global events
    MenuBar::ActionID action = (MenuBar::ActionID) aiDesc._actionItemID;
    MenuBar::SubMenuID subMenuID = (MenuBar::SubMenuID) aiDesc._firstSubmenuID;
    MenuBar::SubMenuID menuID = (MenuBar::SubMenuID) aiDesc._menuID;

    if(action == MenuBar::ActionID::Settings)
    {
        gui::Panel::showOK<SettingsView>(this, (td::UINT4) DialogID::Settings, tr("settings"), this);
//        gui::Dialog::show<SettingsDlg>(this, (td::UINT4) DialogID::Settings);
        return true;
    }
    
    ViewForTab *currentView = dynamic_cast<ViewForTab*>(this->_tabView.getCurrentView());
    
    if (menuID == MenuBar::SubMenuID::Model)
    {
        switch (action)
        {
            case MenuBar::ActionID::EmptyModel:
                addTab<TextEditorView>();
                return true;
            case MenuBar::ActionID::OpenTextModel:
            {
                gui::OpenFileDialog::show(this, tr("openModel"), "*.modl", MenuBar::ActionID::OpenTextModel, [this](gui::FileDialog* d){
                    auto path = d->getFileName();
                    if(path.isNull())
                        return;
                    openFile(path);
                });
//                auto dlg = new gui::OpenFileDialog(this, tr("openModel"), "*.modl");
//
//                dlg->openModal([this](gui::FileDialog* d){
//                    auto path = d->getFileName();
//                    if(path.isNull())
//                        return;
//                    openFile(path);
//                });
                return true;
            }
                
            case MenuBar::ActionID::Import:
            {
                gui::OpenFileDialog::show(this, tr("openModel"), "*.xml", MenuBar::ActionID::Import, [this](gui::FileDialog* d){
                    auto path = d->getFileName();
                    if(path.isNull())
                        return;
                    openFile(path);
                });
                
//                auto dlg = new gui::OpenFileDialog(this, tr("openModel"), "*.xml");
//
//                dlg->openModal([this](gui::FileDialog* d){
//                    auto path = d->getFileName();
//                    if(path.isNull())
//                        return;
//                    openFile(path);
//                });
                return true;
            }
                
            case MenuBar::ActionID::Export:
            {
                if (!currentView)
                    return true;
                gui::SaveFileDialog::show(this, tr("ExporttoXML"), ".xml", MenuBar::ActionID::Export, [currentView](gui::FileDialog* pDlg)
                {
                    td::String path(pDlg->getFileName());
                    if(!path.isNull())
                        currentView->exportToXML(path);
                }, tr("Export"), currentView->getName().c_str());
//                auto path = new gui::SaveFileDialog(this, tr("ExporttoXML"), ".xml", tr("Export"), currentView->getName().c_str());
//                path->openModal([currentView](gui::FileDialog* pDlg) {
//                    td::String path(pDlg->getFileName());
//                    if(!path.isNull()){
//                        currentView->exportToXML(path);
//                    }
//                });
                return true;
            }
                
            case MenuBar::ActionID::Save:
            {
                if (!currentView)
                    return true;
                currentView->save();
                return true;
            }

            case MenuBar::ActionID::SaveAs:
            {
                if (!currentView)
                    return true;
                
                currentView->saveAs();
                return true;
            }
                
            case MenuBar::ActionID::Simulate:
            {
                if (!currentView)
                    return true;
                simulate(currentView);
                return true;
            }
            default:
            {
                if (subMenuID == MenuBar::SubMenuID::NewExampleModel)
                {                    
                    if(action == MenuBar::ActionID::ODE)
                    {
                        openFile(gui::getResFileName(":ODE"));
                        removePath();
                    }
                    else if(action == MenuBar::ActionID::DAE)
                    {
                        openFile(gui::getResFileName(":DAE"));
                        removePath();
                    }
                    else if(action == MenuBar::ActionID::NR)
                    {
                        openFile(gui::getResFileName(":NR"));
                        removePath();
                    }
                    else if(action == MenuBar::ActionID::WLS)
                    {
                        openFile(gui::getResFileName(":WLS"));
                        removePath();
                    }
                    else if(action == MenuBar::ActionID::NRCmplx)
                    {
                        openFile(gui::getResFileName(":NRCmplx"));
                        removePath();
                    }
                    else if(action == MenuBar::ActionID::WLSCmplx)
                    {
                        openFile(gui::getResFileName(":WLSCmplx"));
                        removePath();
                    }
                    return true;
                }
                
            }
        }
    }
    else if (menuID == MenuBar::SubMenuID::Plot)
    {
        auto nElems = _tabView.getNumberOfViews();
        if (nElems == 0)
            return true;
        auto pos = _tabView.getCurrentViewPos();
        DocumentType dt;
        _tabView.getContentType(pos, dt);
        if (dt != DocumentType::Graph)
            return true;
        
        assert (action >= MenuBar::ActionID::PlotFullScreen);
        
        td::UINT2 selection = (td::BYTE) action - (td::BYTE) MenuBar::ActionID::PlotFullScreen;

        return onToolbarsPopoverSelectionChange(nullptr, (td::UINT2) ToolBar::PopOverID::GraphCtrl, selection);
    }
    
//    if(action == MenuBar::ActionID::EmptyModel && subMenuID == MenuBar::SubMenuID::NewGraphical){
////        showStartScreen(false);
//        addTab(new GraphicalEditorView, "");
//        return true;
//    }
//
//
//    if(action == MenuBar::ActionID::EmptyModel && subMenuID == MenuBar::SubMenuID::NewText){
////        showStartScreen(false);
//        addTab(new TextEditorView, "");
//        return true;
//    }


    
    //ViewForTab *currentView = dynamic_cast<ViewForTab*>(this->_tabView.getCurrentView());

//    if(action == MenuBar::ActionID::OpenFromFile){
//        auto &o = *new gui::OpenFileDialog(this, tr("openModel"), {{"Text model editor", "*.modl"}, {"XML model", "*.xml"}, {tr("tfFile"), "*.tfstate"} }, tr("open"));
//        if(menuID == MenuBar::SubMenuID::Model){
//           if(currentView == nullptr)
//                return true;
//            o.openModal([currentView](gui::FileDialog* d){
//                auto path = d->getFileName();
//                if(path.isNull())
//                    return;
//                currentView->loadFile(path);
//            });
//        }
//        if(menuID == MenuBar::SubMenuID::NewModel){
//                 o.openModal([this](gui::FileDialog* d){
//            auto path = d->getFileName();
//            if(path.isNull())
//                return;
//            
//            openFile(path);
//
//        });   
//            return true;
//        }
//    }


//   if(currentView == nullptr) //ostale akcije nemaju smisla ako nema ni jedan tab otvoren
//        return true;



//    if(action == MenuBar::ActionID::Export){
//        auto path = new gui::SaveFileDialog(this, tr("ExporttoXML"), ".xml", tr("Export"), currentView->getName().c_str());
//        path->openModal([currentView](gui::FileDialog* pDlg) {
//            td::String path(pDlg->getFileName());
//            if(!path.isNull()){
//                currentView->exportToXML(path);
//            }
//        });
//        return true;
//    }


  
    return false;
}

bool MainWindow::onToolbarsPopoverSelectionChange(gui::PopoverView* pPOView, td::UINT2 ctrlID, td::UINT2 selection)
{
    if (ctrlID == (td::UINT2) ToolBar::PopOverID::NewFromTemplate)
    {
        switch (selection)
        {
            case 0:
                openFile(gui::getResFileName(":NR"));
                removePath();
                return true;

            case 1:
                openFile(gui::getResFileName(":WLS"));
                removePath();
                return true;
            case 2:
                openFile(gui::getResFileName(":NRCmplx"));
                removePath();
                return true;
            case 3:
                openFile(gui::getResFileName(":WLSCmplx"));
                removePath();
                return true;
            case 4:
                openFile(gui::getResFileName(":ODE"));
                removePath();
                return true;
            case 5:
                openFile(gui::getResFileName(":DAE"));
                removePath();
                return true;
            default:
                assert(false);
        }
    }
    else if (ctrlID == (td::UINT2) ToolBar::PopOverID::GraphCtrl)
    {
        auto nView = _tabView.getNumberOfViews();
        if (nView == 0)
            return true;
        
        int pos = _tabView.getCurrentViewPos();
        DocumentType dt = DocumentType::Unknown;
        _tabView.getContentType(pos, dt);
        if (dt != DocumentType::Graph)
            return true;
        auto view = _tabView.getView(pos);
        gui::plot::View* pView = dynamic_cast<gui::plot::View*>(view);
        //assert(pView);
        if(pView != nullptr)
            pView->handleAction((gui::plot::View::Action) selection);
        return true;
    }
    
    return false;
}


bool MainWindow::onChangedSelection(gui::TabView* pNavigator)
{
    auto nViews = pNavigator->getNumberOfViews();
    if (nViews == 0)
    {
        _mainMenuBar.enableTxtOptions(false);
        _mainMenuBar.enablePlotMenu(false);
        _toolBar.enableTxtOptions(false);
        _toolBar.enablePlot(false);
        _prevDocType = DocumentType::Unknown;
        return true;
    }
    
    auto pos = pNavigator->getCurrentViewPos();
    DocumentType docType = DocumentType::Unknown;
    pNavigator->getContentType(pos, docType);
    if (_prevDocType == docType)
        return true;
    
    switch(docType)
    {
        case DocumentType::ModelTxtEditor:
            _mainMenuBar.enableTxtOptions(true);
            _toolBar.enableTxtOptions(true);
            _mainMenuBar.enablePlotMenu(false);
            _toolBar.enablePlot(false);
            break;
        case DocumentType::Graph:
            _mainMenuBar.enableTxtOptions(false);
            _toolBar.enableTxtOptions(false);
            _mainMenuBar.enablePlotMenu(true);
            _toolBar.enablePlot(true);
            break;
        default:
            break;
            
    }
    _prevDocType = docType;
    return true;
}

bool MainWindow::openFile(const td::String& path)
{

    if (path.endsWith(".modl") || path.endsWith(".xml"))
        addTab<TextEditorView>(path);

    if (path.endsWith(".tfstate"))
        addTab<GraphicalEditorView>(path);
    


    return false;

}

void MainWindow::onInitialAppearance()
{

    auto &font = GlobalEvents::settingsVars.font;
    if(font.cCompareNoCase("Default") == 0)
    {

#ifdef MU_MACOS
        td::String preferedFont("Menlo");
#elif MU_WINDOWS
        td::String preferedFont("Consolas");
#else
        td::String preferedFont("DejaVu Sans Mono");
#endif
        auto monospacedFonts = gui::Font::getSystemMonospacedFamilyNames();
        bool foundDefault = false;
        for (const auto& fnt : monospacedFonts)
        {
            if (fnt == preferedFont)
            {
                foundDefault = true;
                break;
            }
        }
        
        if (!foundDefault)
            font = monospacedFonts[0];
        else
            font = preferedFont;
            

    }
    else //provjera da li je izabrani font monospace. Mada ovo se ne moze nikad desiti jer se samo monospace fontovi mogu izabrati
    {
        auto monospacedFonts = gui::Font::getSystemMonospacedFamilyNames();
        bool foundRequired = false;
        for (const auto& fnt : monospacedFonts)
        {
            if (fnt == font)
            {
                foundRequired = true;
                break;
            }
        }
        
        if (!foundRequired)
            font = monospacedFonts[0];
    }

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


    if (getAttachedWindow((td::UINT4)DialogID::DataDrawer) == nullptr){
        dataDrawerPtr = new DataDraw; //workaround, view koji se nalazi u gui::Window prestaje biti validan nakon njegovog gasenja
        auto ptr = new DataDrawerWindow(this, dataDrawerPtr);
        ptr->setTitle("Simulation results");
        ptr->open();
    }


    return dataDrawerPtr;
}


const ModelNode &MainWindow::getModelFromTabOrFile(const td::String &modelNameOrPath)
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
    }
    else
    { //tab
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

    static ModelNode m; // should never run
    return m;
}

bool MainWindow::prepareForClose()
{
    
//    decltype(_tabsToProcess)::iterator it;
//    ViewForTab* tab;
    
    //check if there is any unsaved
    bool bModified = false;
    for (int i = _tabView.getNumberOfViews() - 1; i >= 0; --i)
    {
        //IDz: nema potrebe za dodatnom mapom
//        it = _tabsToProcess.find(_tabView.getView(i));
//        if (it == _tabsToProcess.end())
//            continue;

//        tab = (ViewForTab*)*it;
        
        DocumentType dt;
        _tabView.getContentType(i, dt);
        if (dt != DocumentType::ModelTxtEditor)
            continue;
        
//        ViewForTab* tab = (ViewForTab*) _tabView.getView(i);
//        if (tab->isModified())
        if (_tabView.isModified(i))
        {
            bModified = true;
            break;
        }
    }
    
    if (!bModified)
        return true;

    auto diag = new SaveDlg(this);

    diag->setResizable(false);
    diag->keepOnTopOfParent();
        
    diag->openModal([this](gui::Dialog::Button::ID id, gui::Dialog* d){
        bool bClose = true;
        if(id == gui::Dialog::Button::ID::OK) //save
        {
            for (int i = _tabView.getNumberOfViews() - 1; i >= 0; --i)
            {
//                decltype(_tabsToProcess)::iterator it;
//                ViewForTab* tab;
                
//                it = _tabsToProcess.find(_tabView.getView(i));
//                if (it == _tabsToProcess.end())
//                    continue;

                //tab = (ViewForTab*)*it;
                
                DocumentType dt;
                _tabView.getContentType(i, dt);
                if (dt != DocumentType::ModelTxtEditor)
                    continue;
                
                //TODO: Izbaciti komplikaciju sa _tabsToProcess.find
                if (_tabView.isModified(i))
                {
                    ViewForTab* tab = (ViewForTab*) _tabView.getView(i);
                    tab->save();
                }
            }
        }
        else if (id == gui::Dialog::Button::ID::Cancel) //do not save return
            bClose = false;
        
        
        if (bClose)
        {
//            _closeWindow = true;
            this->close();
            return;
        }
        
        //if(id == gui::Dialog::Button::ID::Delete || id == gui::Dialog::Button::ID::OK) //dont save
            //GlobalEvents::getMainWindowPtr()->closeTab(this);

//        if (exitProgram && id != gui::Dialog::Button::ID::Delete) {
//            GlobalEvents::getMainWindowPtr()->prepareForClose();
//        }

    });
    

//    for (int i = _tabView.getNumberOfViews() - 1; i >= 0; --i)
//    {
//        it = _tabsToProcess.find(_tabView.getView(i));
//        if (it == _tabsToProcess.end())
//            continue;
//
//        tab = (ViewForTab*)*it;
//
//        _tabsToProcess.erase(it);
//        _tabView.showView(i);
//        if (tab->promptSaveIfNecessary(true))
//            return false;
//
//    }

    return false;
}



bool MainWindow::shouldClose()
{
    auto pDlg = getAttachedWindow((td::UINT4)MainWindow::DialogID::SaveData);
    if (pDlg)
        return false; //dialog is open and we wait on the user's reaction
    
    if(GlobalEvents::settingsVars.warnBeforeClose == false && GlobalEvents::settingsVars.restoreTabs == false)
        return true;

//    if (_closeWindow)
//        return true;

    //Zasto ovolike komplikacije
    //Zasto skupljati tabove u posebnu listu kad su svi vec dostupni
    
    
    //TODO: dodati ovdje ako vidljivost dijalog
    
//    if (!_tabsToProcess.empty()) {
//        return prepareForClose();
//    }

    ViewForTab* tab;
    cnt::StringBuilder pathBuilder;

    for (int i = 0; i < _tabView.getNumberOfViews(); ++i)
    {
        tab = dynamic_cast<ViewForTab*>(_tabView.getView(i));
        if(tab == nullptr)
            continue;

        if(GlobalEvents::settingsVars.restoreTabs && !tab->getPath().isNull())
            pathBuilder << tab->getPath() << ":";

        //if (GlobalEvents::settingsVars.warnBeforeClose)
          // _tabsToProcess.emplace(tab); //zar nije jednostavnije napraviti metod isModified umjesto set-a???

    }

    pathBuilder.getString(_paths);
    getAppProperties()->setValue("previousSessionTabPaths", _paths);
    GlobalEvents::settingsVars.saveValues();


    if (GlobalEvents::settingsVars.warnBeforeClose)
    {
        return prepareForClose();
    }

    return true;
}

MainWindow::~MainWindow()
{

    
}

bool MainWindow::handleUserEvent(td::UINT4 eventID, const td::Variant& userParam)
{
    if(_tabView.getNumberOfViews() == 0)
        return true;
    
    UserEvent event = (UserEvent) eventID;

    switch (event)
    {
        case UserEvent::ReformatTabs:
        {
            auto view = dynamic_cast<ViewForTab*>(_tabView.getCurrentView());
            if (!view)
                return false;
            view->handleColorSettings();
        }
            break;
        case UserEvent::SetModifiedFlag:
        {
            int iPos = _tabView.getCurrentViewPos();
            if (iPos < 0)
                return false;
            _tabView.setModified(iPos, userParam.boolVal());
        }
            break;
        case UserEvent::CloseTab:
        {
            int iPos = _tabView.getCurrentViewPos();
            if (iPos < 0)
                return false;
            gui::BaseView* bv = (gui::BaseView*) userParam.voidPtr();
            closeTab(bv);
        }
            break;
    }
    
    return true;
}

DataDrawerWindow::DataDrawerWindow(gui::Window *parent, DataDraw* mainView):
    gui::Window(gui::Size(500, 500), parent, (td::UINT4)MainWindow::DialogID::DataDrawer),
    storedView(mainView)
{
    setCentralView(storedView);
}

DataDrawerWindow::~DataDrawerWindow()
{
    delete storedView;
}
