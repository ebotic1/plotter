#include "View.h"
#include "td/String.h"

#include "td/Variant.h"
#include "td/StringConverter.h"
#include "gui/NumericEdit.h"
#include "./../../EquationToXML/src/main.h"

#include "xml/Writer.h"

#include <cstdlib>
#include <Windows.h>









View::View() : hLayout(1), vLayout(9), jednacinaLabel("Jednacina:"), _generisiXML("Kreiraj XML"), dugmadi(2), opcije(5), timeLayout(7), 
starLabel("pocetno vrijeme simulacije"), endLabel("krajnje vrijeme simulacije"), korakLabel("korak: "), _start(td::DataType::decimal2), _end(td::DataType::decimal2), _step(td::DataType::decimal2), _generisiTXT("Kreiraj rezultat.txt")
{


	//hLayout.appendLayout(vLayout);

	_end.setText("10");
	_step.setText("0.01");

	vLayout.appendLayout(opcije);
	timeLayout << starLabel << _start << endLabel << _end << korakLabel << _step;
	timeLayout.appendSpace(20);
	vLayout.appendSpace(20);
	vLayout.appendLayout(timeLayout);
	vLayout.appendSpace(30);



	vLayout.append(jednacinaLabel, td::HAlignment::Left);
	vLayout.append(_jednacineText);

	_jednacineText.setText("c=5\nx'=c");

	dugmadi.append(_generisiXML, td::HAlignment::Center);
	dugmadi.append(_generisiTXT, td::HAlignment::Center);


	vLayout.appendSpace(50);
	vLayout.appendLayout(dugmadi);
	vLayout.appendSpace(10);

	
	
	vLayout.setMargins(20, 20);
	setLayout(&vLayout);

	
}


bool View::onClick(gui::Button* pBtn)
{

	if (pBtn == &_generisiXML) {
		td::Variant v;
		_jednacineText.getValue(v);
		
		const char* userProfile = getenv("USERPROFILE");
		td::String path(userProfile);
		path += "\\modelSolver\\test.xml";

		try
		{
			generateXML(v.strVal(), path);
		}
		catch (const std::exception& e)
		{
			
			return true;
		}

		

	}

	if (pBtn == &_generisiTXT) {
		pokreniSolver();

	}
	return false;
}




void View::pokreniSolver(){

	td::String s;
	const char* userProfile = getenv("USERPROFILE");
	s = "cd ";
	s += userProfile;
	s += "\\modelSolver & .\\modelSolver.exe ";
	//s += tip.getSelectedText();
	s += " ";
	//s += domena.getSelectedText();
	s += " test.xml";
	s += " rezultat.txt ";
	td::Variant v;

	_start.getValue(v);
	s += v.toString();
	s += " ";
	
	_step.getValue(v);
	s += v.toString();
	s += " ";

	_end.getValue(v);
	s += v.toString();
	s += " & pause";


	system(s.c_str());



	
}



