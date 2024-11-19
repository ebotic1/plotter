#include <gui/View.h>
#include <gui/GridLayout.h>
#include <gui/ComboBox.h>
#include <gui/GridComposer.h>
#include <gui/Slider.h>
#include <gui/Label.h>
#include "../inc/Graph.h"

class Settings : public gui::View{
    gui::GridLayout _grid;
    gui::Label _legendColsLabel;
    gui::ComboBox _comboCols;

    gui::Label _marginLeftLbl, _marginRightLbl, _marginTopLbl, _marginBottomLbl;
    gui::Slider _marginLeftSlider, _marginRightSlider, _marginTopSlider, _marginBottomSlider;

    double _margins[4];

    gui::Label _xSpaceLbl, _ySpaceLbl;
    gui::Slider _xSpaceSlider, _ySpaceSlider;

    double _axisSpace[2];

public:
    Settings(Graph *parent):
    _grid(8,2),
    _legendColsLabel(tr("plotter_legendCols")),
    _marginLeftLbl(tr("plotter_marginLeft")),
    _marginRightLbl(tr("plotter_marginRight")),
    _marginTopLbl(tr("plotter_marginTop")),
    _marginBottomLbl(tr("plotter_marginBottom")),

    _xSpaceLbl(tr("plotter_xSpace")),
    _ySpaceLbl(tr("plotter_ySpace"))
    {
        gui::GridComposer gc(_grid);
        _comboCols.addItem("1");
        _comboCols.addItem("2");
        _comboCols.addItem("3");
        _comboCols.addItem("4");

        _comboCols.onChangedSelection([parent, this](){parent->setLegendCols(_comboCols.getSelectedIndex()+1);});

        gc.appendRow(_legendColsLabel) << _comboCols;

        parent->getMargins(_margins[0], _margins[1], _margins[3], _margins[2]);
        _marginLeftSlider.setValue(_margins[0], false);
        _marginRightSlider.setValue(_margins[1], false);
        _marginTopSlider.setValue(_margins[2], false);
        _marginBottomSlider.setValue(_margins[3], false);

        _marginLeftSlider.setRange(0, 200);
        _marginRightSlider.setRange(0, 200);
        _marginTopSlider.setRange(0, 200);
        _marginBottomSlider.setRange(0, 200);

        _marginLeftSlider.onChangedValue([this, parent](){td::Variant v;_marginLeftSlider.getValue(v); _margins[0] = v.r8Val(); parent->setMargins(_margins[0], _margins[1], _margins[3], _margins[2]);});
        _marginRightSlider.onChangedValue([this, parent](){td::Variant v;_marginRightSlider.getValue(v);  _margins[1] = v.r8Val(); parent->setMargins(_margins[0], _margins[1], _margins[3], _margins[2]);});
        _marginTopSlider.onChangedValue([this, parent](){td::Variant v;_marginTopSlider.getValue(v);  _margins[2] = v.r8Val(); parent->setMargins(_margins[0], _margins[1], _margins[3], _margins[2]);});
        _marginBottomSlider.onChangedValue([this, parent](){td::Variant v;_marginBottomSlider.getValue(v);  _margins[3] = v.r8Val(); parent->setMargins(_margins[0], _margins[1], _margins[3], _margins[2]);});

        gc.appendRow(_marginLeftLbl) << _marginLeftSlider;
        gc.appendRow(_marginRightLbl) << _marginRightSlider;
        gc.appendRow(_marginTopLbl) << _marginTopSlider;
        gc.appendRow(_marginBottomLbl) << _marginBottomSlider;

        gc.startNewRowWithSpace(0, 10);

        parent->getAxisNameDistance(_axisSpace[0], _axisSpace[1]);

        _xSpaceSlider.setValue(_axisSpace[0], false);
        _ySpaceSlider.setValue(_axisSpace[1], false);

        _xSpaceSlider.setRange(20, 200);
        _ySpaceSlider.setRange(20, 200);

        _xSpaceSlider.onChangedValue([this, parent](){td::Variant v; _xSpaceSlider.getValue(v); _axisSpace[0] = v.r8Val(); parent->setAxisNameDistance(_axisSpace[0], _axisSpace[1]);});
        _ySpaceSlider.onChangedValue([this, parent](){td::Variant v; _ySpaceSlider.getValue(v); _axisSpace[1] = v.r8Val(); parent->setAxisNameDistance(_axisSpace[0], _axisSpace[1]);});

        gc.appendRow(_xSpaceLbl) << _xSpaceSlider;
        gc.appendRow(_ySpaceLbl) << _ySpaceSlider;


        setLayout(&_grid);
    }


};