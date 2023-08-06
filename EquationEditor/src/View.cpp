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
	static std::string line;

	if (path != nullptr) {
		file.close();
		file.open(path);
		std::getline(file, std::string());
		std::getline(file, line);
		return file.is_open();
	}
	if (!file.is_open() || file.eof()) {
		alone = false;
		return false;
	}

	comment.clean();

	//check if multiple elements are on the same line, only the last one registers as having a comment
	int pozComment, pozStart, pozEnd;
	static int poz = 0;
	while (true) {
		if (file.eof()) {
			alone = false;
			return true;
		}
		
		pozComment = line.find("<!--", poz);
		pozStart = line.find("<", poz);
		pozEnd = line.find(">", poz);

		if (pozStart != -1 && line[pozStart + 1] == '/') {//closing tag (comment is independent, ignore)

			if (pozComment != -1) {
				poz = pozComment;
				bool getSucces = getComment(comment, alone);
				alone = true;
				return getSucces;
			}

			//komentara nema, samo nadi kraj elementa i zapocni ponovo
			while (poz = line.find(">", poz), (poz != -1 && !file.eof()))
				std::getline(file, line);


			if (file.eof()) {
				alone = false;
				comment.clean();
				return false;
			}

			poz += 1;
			continue;
		}

		if (pozComment == -1) { //nema komentara
			if (pozEnd == -1) { //nema elementa
				std::getline(file, line);
				poz = 0;
				continue;
			}
			else {//ima elementa
				poz = pozEnd + 1;
				alone = true;
				return true;
			}

		}

		if (pozComment < pozStart) { //komenatar prije pocetka elementa
			alone = true;
			break;
		}
		//komentar iza elementa

		alone = false;
		break;

	}


	pozComment += 4;
	int poz2;
	

	while ((poz2 = line.find("-->")), poz2 == -1) {
		comment += line.substr(pozComment);
		//if (line.size() != 0) 
			comment += "\n//";
		pozComment = 0;
		if (!std::getline(file, line))
			return false;
	}
	comment += line.substr(pozComment, poz2 - pozComment);

	poz = poz2 + 3;

	return true;
}




void printNode(StringBuilder &result, xml::FileParser::node_iterator &node, int tabs = 0) {

	td::String comment;

	bool alone;
	getComment(comment, alone);

	while (alone) {
		result << "//" << comment << '\n';
		getComment(comment, alone);
	}

	static std::string *tab_storage = new std::string; //u sustini ovaj string se nikad ne brise iz memorije ali mozda bolje to nego da se nanovo memorija rezervise svaki put kad pozivamo funkciju
	tab_storage->resize(tabs, '\t');

	result << tab_storage->c_str();

	bool special_case = false;
	bool skip_newline = false;
	enum class closeTags{none, model, group, iff} closeTag = closeTags::none;


	if (node->getName().cCompareNoCase("Model") == 0)
		closeTag = closeTags::model;

	if (node->getName().cCompareNoCase("Group") == 0)
		closeTag = closeTags::group;
	
	if (node->getName().cCompareNoCase("Var") == 0 || node->getName().cCompareNoCase("Param") == 0) {
		result << tab_storage->c_str() << '\t';

		auto atrib = node.getAttrib("name");
		if (atrib != nullptr)
			result << atrib->getValue();
		else
			result << "name_not_specified";
		result << " = ";


		atrib = node.getAttrib("val");
		if (atrib != nullptr)
			result << atrib->getValue();
		else
			result << "0";

		goto PRINT_CHILDREN;
	}
	
	
	if (node->getName().cCompareNoCase("Then") == 0) {
		special_case = true;
		skip_newline = true;
	}


	if (node->getName().cCompareNoCase("Else") == 0) {
		result << tab_storage->c_str() << "else";
		special_case = true;
	}

	bool ifType = false;

	if (node->getName().cCompareNoCase("Eq") == 0) { // ako se radi o uslovnoj jednacini ispisati kao if
		auto atr = node.getAttrib("cond");
		if(atr != nullptr){
			result << "if " << atr->getValue();
			special_case = true;
			closeTag = closeTags::iff;
			ifType = true;
		}
	}
	else if(!special_case) 
		result << tab_storage->c_str() << node->getName() << ':';

		
	//printanje argumenata

	auto atr = node.getAttrib("fx");
	if (atr != nullptr) {
		if (ifType)
			result << '\n';
		result << tab_storage->c_str() << atr->getValue();
		atr = node.getAttrib("w");
		if (atr != nullptr)
			result << " [" << atr->getValue() << ']';
	}


	for each (auto & var in node->attribs) {
		if (ifType && var.getName().cCompareNoCase("cond") == 0)
			continue;		
		if (var.getName().cCompareNoCase("fx") == 0)
			continue;
		if (var.getName().cCompareNoCase("w") == 0)
			continue;
		result << '\n' << tab_storage->c_str() << '\t' << var.getName() << " = " << var.value;
	}

	
	
PRINT_CHILDREN:
	if (!comment.isNull()) 
		result << "\t//" << comment;
	if (!skip_newline)
		result << '\n';
	

	auto &it = node.getChildNode();
	while (it.isOk()) {
		printNode(result, it, tabs);
		++it;
	}



	static const char* closingTagNames[] = { "" , "endmodel\n", "endgroup\n", "endif\n"};
	result << closingTagNames[int(closeTag)];

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
	}
	return true;
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
	}
	return true;
}

