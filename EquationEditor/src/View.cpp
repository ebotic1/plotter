#include "View.h"
#include "td/String.h"

#include "td/Variant.h"
#include "td/StringConverter.h"
#include "gui/NumericEdit.h"
#include "./../../EquationToXML/src/main.h"

#include "xml/DOMParser.h"
#include "cnt/StringBuilder.h"
#include <fstream>

#include <cstdlib>
#include <Windows.h>









View::View() : hLayout(1), vLayout(9), jednacinaLabel("Jednacina:"), dugmadi(1), opcije(5), timeLayout(7), 
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

	dugmadi.append(_generisiTXT, td::HAlignment::Center);


	vLayout.appendSpace(50);
	vLayout.appendLayout(dugmadi);
	vLayout.appendSpace(10);

	
	
	vLayout.setMargins(20, 20);
	setLayout(&vLayout);

	
}


bool View::onClick(gui::Button* pBtn)
{

	if (pBtn == &_generisiTXT) {
		pokreniSolver();

	}
	return false;
}




void View::pokreniSolver(){

	xml::FileParser p;
	p.parseFile(currentPath);
	auto root = p.getRootNode();

	td::String s; //treba string builder ali ovo je samo privremeno dok ne bude API
	const char* userProfile = getenv("USERPROFILE");
	s = "cd ";
	s += userProfile;
	s += "\\modelSolver & .\\modelSolver.exe ";
	s += root.getAttrib("type")->getValue();
	s += " ";
	s += root.getAttrib("domain")->getValue();
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

bool getComment(td::String& comment, bool &alone, const char* path = nullptr) {
	static std::ifstream file;
	if (path != nullptr) {
		file.close();
		file.open(path);
		std::getline(file, std::string());
		return file.is_open();
	}
	if (!file.is_open())
		return false;

	std::string line;
	if (!std::getline(file, line)) {
		comment.clean();
		alone = false;
		return false;
	}

	td::String TDline(line);
	TDline = std::move(TDline.trim());

	{ //ako je linija samo zatvaranje elementa i eventualno komentar onda se ona preskace u xml parseru pa se vraca i rezultat iduce line
		int poz1 = TDline.find("<!--");
		int poz2 = TDline.find("-->");
		bool commentPresent = false;
		if (poz1 != -1 && poz2 != -1) {
			//TDline = TDline.subStr(0, poz1) + TDline.subStr(poz2, TDline.length() - 1); compiler error?
			td::String s1 = TDline.subStr(0, poz1);
			td::String s2 = TDline.subStr(poz2, TDline.length() - 1);
			s1 += s2;
			TDline = std::move(s1);
			commentPresent = true;
		}
		if (TDline.find("</") == 0 && TDline.find(">") == (TDline.length() - 1)) {
			comment.clean();
			if(commentPresent)
				comment += td::String(line).trim().subStr(poz1+4, poz2);
			td::String comment2;
			bool getSucces = getComment(comment2, alone);
			comment += comment2;
			return getSucces;
		}
	}
		
	int poz = td::String(line).trimLeft().find("<!--");
	if (poz == -1) {
		comment = "\0";
		alone = false;
		return true;
	}
	if (poz == 0)
		alone = true;
	else
		alone = false;
	poz = line.find("<!--") + 4;
	int poz2;
	comment.clean();
	while ((poz2 = line.find("-->")), poz2 == -1) {
		comment += line.substr(poz);
		poz = 0;
		if (!std::getline(file, line))
			return false;
	}
	comment += line.substr(poz, poz2-poz);

	return true;
}


void printNode(StringBuilder &result, xml::FileParser::node_iterator &node, int tabs = 0) {

	const char* smece = node->getName().c_str();
	td::String comment;

	auto finishLine = [&comment, &result]() {
		if (comment.isNull())
			result << '\n';
		else
			result << "\t//" << comment << '\n';
	};

	bool alone;
		getComment(comment, alone);
	while(alone){
		result << "//" << comment << '\n';
		getComment(comment, alone);
	}

	static std::string *tab_storage = new std::string; //u sustini ovaj string se nikad ne brise iz memorije ali mozda bolje to nego da se nanovo memorija rezervise svaki put kad pozivamo funkciju
	tab_storage->resize(tabs+1, '\t');
	(*tab_storage)[tabs] = '\0';

	result << tab_storage->c_str();

	bool ifType = false;
	bool special_case = false;
	
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
				result << var.value;
				found = true;
				break;
			}
		if (!found)
			result << '0';
		finishLine();
		goto PRINT_CHILDREN;
	}
	
	
	if (node->getName().cCompareNoCase("Then") == 0) { //then je implicitno ne ispisuje se
		finishLine();
		goto PRINT_CHILDREN;
	}


	if (node->getName().cCompareNoCase("Eq") == 0) { // ako se radi o uslovnoj jednacini ispisati kao if
		td::String eq;
		for each (auto & var in node->attribs) {
			if (var.getName().cCompareNoCase("cond") == 0) {
				result << "if " << var.value;
				ifType = true;
			}
			if (var.getName().cCompareNoCase("fx") == 0)
				eq = var.value;
		}
		if (!ifType) {
			tab_storage[tabs] = '\t';
			result << tab_storage->c_str() << eq;
			finishLine();
			goto PRINT_CHILDREN;
		}
		special_case = true;
	}

	if (!special_case)
		result << tab_storage->c_str() << node->getName() << ':';
	

		
	tab_storage[tabs] = '\t';

	finishLine();
		
	for each (auto & var in node->attribs) {
		if (ifType && var.getName().cCompareNoCase("cond") == 0)
			continue;			
		result << tab_storage->c_str() << var.getName() << " = " << var.value << '\n';
	}

	
	
PRINT_CHILDREN:
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
	bool x;

	if (!par.parseFile(path) || !getComment(td::String(), x, path.c_str())) {
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
		td::String comment;
		while (getComment(comment, x))
			s << "//" << comment << '\n';
	}

	

	_jednacineText.clean();
	currentPath = path;
	td::String temp;
	s.getString(temp);//moglo bi bez temp da getString vraca string pa da se direktno std::move
	_jednacineText.setText(std::move(temp));

}

bool View::saveXML()
{
	try
	{
		generateXML(_jednacineText.getText(), currentPath);
	}
	catch (const std::exception& e)
	{
		showAlert("Could not save file", e.what());
		return true;
	}
}

bool View::saveAsXML(const td::String& path)
{
	try
	{
		generateXML(_jednacineText.getText(), path);
	}
	catch (const std::exception& e)
	{
		showAlert("Could not save file", e.what());
		return true;
	}
}

