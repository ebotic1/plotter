#include "gui/View.h"
#include "gui/Dialog.h"
#include "gui/ComboBox.h"


class txtDiag : public gui::Dialog {
    
public:
    bool horizontal = true;
    txtDiag(gui::Frame* pFrame, td::UINT4 wndID = 0);
    ~txtDiag();


};