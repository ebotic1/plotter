#include "View.h"
#include "td/String.h"

#include "td/Variant.h"
#include "td/StringConverter.h"
#include "gui/NumericEdit.h"
#include "./../../EquationToXML/src/main.h"

#include "xml/DOMParser.h"
#include "cnt/StringBuilder.h"

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

	_jednacineText.setText("Model:\ntype = NR;\ndomain = real;\nname = trn;\nvars:\nx = 0;\nparams:\nk = 1;\nNL:\nx = k;");

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

	td::String s; //treba string builder ali ovo je samo privremeno dok ne bude API
	const char* userProfile = getenv("USERPROFILE");
	s = "cd ";
	s += userProfile;
	s += "\\modelSolver & .\\modelSolver.exe ";
	s += "NR";
	s += " ";
	s += "real";
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

typedef cnt::StringBuilder<td::String, 1024*64> StringBuilder;




void printNode(StringBuilder &result, xml::FileParser::node_iterator &node, int tabs = 0) {

	const char* smece = node->getName().c_str();

	static std::string *tab_storage = new std::string; //u sustini ovaj string se nikad ne brise iz memorije ali mozda bolje to nego da se nanovo memorija rezervise svaki put kad pozivamo funkciju
	tab_storage->resize(tabs+1, '\t');
	(*tab_storage)[tabs] = '\0';

	result << tab_storage->c_str();

	bool skipAttrib = false;
	bool ifType = false;
	
	if (node->getName().cCompareNoCase("Var") == 0 || node->getName().cCompareNoCase("Param") == 0) {
		tab_storage[tabs] = '\t';
		for each (auto & var in node->attribs)
			if (var.getName().cCompareNoCase("name") == 0) {
				result << tab_storage->c_str() << var.getValue() << " = ";
				break;
			}
		bool found = false;
		for each (auto & var in node->attribs)
			if (var.getName().cCompareNoCase("val") == 0) {
				result << var.value << "\n";
				found = true;
				break;
			}
		if (!found)
			result << "0\n";
		skipAttrib = true;
	}else if (node->getName().cCompareNoCase("Eq") == 0) { // ako se radi o uslovnoj jednacini ispisati kao if
		for each (auto & var in node->attribs) {
			if (var.getName().cCompareNoCase("cond") == 0) {
				result << "if " << var.value << "\n";
				ifType = true;
			}
		}

	}
	else if (node->getName().cCompareNoCase("Then") == 0); //then je implicitno ne ispisuje se
	else
		result << tab_storage->c_str() << node->getName() << ":\n";
	
	if (!skipAttrib) {
		tab_storage[tabs] = '\t';
		for each (auto & var in node->attribs) {
			if (ifType && var.getName().cCompareNoCase("cond") == 0)
				continue;
			if (var.getName().cCompareNoCase("fx") == 0)
				result << tab_storage->c_str() << var.value << "\n";
			else
				result << tab_storage->c_str() << var.getName() << " = " << var.value << "\n";
		}
	}

	

	auto &it = node.getChildNode();
	while (it.isOk()) {
		printNode(result, it, tabs);
		++it;
	}
	return;
}

void View::loadXML(td::String path){
	xml::FileParser par;
	StringBuilder s;

	if (!par.parseFile(path)) {
		showAlert("error", "Cant open file");
		return;
	}

	

	auto &model = par.getRootNode();

	
	

	if (model->getName().cCompare("Model") != 0) {
		showAlert("error", "File not valid");
		return;
	}
	else {
		printNode(s, model);
	}

	

	_jednacineText.clean();
	td::String temp;
	s.getString(temp);//moglo bi bez temp da getString vraca string pa da se direktno std::move
	_jednacineText.setText(std::move(temp));

}



