#include "gui/View.h"
#include "gui/Dialog.h"
#include <vector>
#include "gui/VerticalLayout.h"
#include "gui/Label.h"
#include "../../common/property.h"

#include "gui/NumericEdit.h"


class annotDiag : public gui::Dialog {

    gui::VerticalLayout *verLay = nullptr;
    gui::Label horLabel, varLabel;
    std::vector<elementProperty*> props;
    gui::View view;

public:

	annotDiag(gui::Frame* pFrame, std::vector<gui::CoordType> &verticals, std::vector<gui::CoordType>& horizontals, td::UINT4 wndID = 0) : 
    gui::Dialog(pFrame, { {gui::Dialog::Button::ID::Close, "Close"}}, gui::Size(500, 100), wndID),
    horLabel("Horizontal annotations:"), varLabel("Vertical annotations:")
    {	
    
        int size = verticals.size() + horizontals.size() + 2;
        verLay = new gui::VerticalLayout(size);

        verLay->append(varLabel);
        int cnt = 0;
        while (cnt < 2) {
            std::vector<gui::CoordType>& vector = (cnt == 0) ? verticals : horizontals;

            if (cnt == 1)
                verLay->append(horLabel);

            for (size_t i = 0; i < vector.size(); ++i) {
                td::String name;
                name.format("line%d: ", i + 1);
                td::Variant v(vector[i]);
                auto ptr = new elementProperty(name, v.getType(), "x axis value that the vertical line is crossing", v);
                gui::CoordType& val = vector[i];
                ptr->Action = [&val](const td::Variant& v) {
                    v.getValue(&val, td::real8);
                };

                props.push_back(ptr);
                verLay->append(*ptr);
            }
            ++cnt;
        }

        view.setLayout(verLay);
        
        setCentralView(&view);
    }

    ~annotDiag() {
        for (size_t i = 0; i < props.size(); ++i) {
            delete props[i];
        }
        delete verLay;
    }


};