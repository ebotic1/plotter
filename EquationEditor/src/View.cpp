#include "View.h"
#include "td/String.h"

#include "td/Variant.h"
#include "td/StringConverter.h"
#include "gui/NumericEdit.h"
#include "./../../EquationToXML/inc/generateXML.h"

#include "xml/DOMParser.h"
#include "cnt/StringBuilder.h"
#include <fstream>

#include <cstdlib>
#include <Windows.h>









View::View() : splitter(gui::SplitterLayout::Orientation::Horizontal), vLayout(9), jednacinaLabel("Jednacina:"), dugmadi(1), opcije(5), timeLayout(7),
starLabel("pocetno vrijeme simulacije"), endLabel("krajnje vrijeme simulacije"), korakLabel("korak: "), _start(td::DataType::decimal2), _end(td::DataType::decimal2), _step(td::DataType::decimal2), _generisiTXT("Run")
{


	_end.setText("10");
	_step.setText("0,01");

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


	vLayout.appendSpace(20);
	vLayout.appendLayout(dugmadi);
	vLayout.appendSpace(30);
	
	vLayout.setMargins(20, 20);
	splitter.setContent(vLayout, plotter);

	setLayout(&splitter);

	
}



bool extractNumbers(const std::string& path, //chatGPT
	std::vector<std::string>& headers,
	std::vector<std::vector<double>>& data) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "Failed to open file." << std::endl;
		return false;
	}

	std::string line;
	bool startData = false;

	while (getline(file, line)) {
		// Start collecting data after finding "SOLUTION DATA"
		if (line.find("SOLUTION DATA") != std::string::npos) {
			// Skip the divider line
			getline(file, line);
			// Read the header line
			getline(file, line);

			// Split headers and save them
			std::istringstream issHeaders(line);
			std::string header;
			while (issHeaders >> header) {
				headers.push_back(header);
			}

			// Initialize data vectors based on header count
			data.resize(headers.size());

			// Skip the divider line
			getline(file, line);
			startData = true;
			continue;
		}

		if (startData && !line.empty()) {
			std::istringstream issData(line);
			double value;
			int colIdx = 0;
			while (issData >> value) {
				if (colIdx < data.size()) {
					data[colIdx].push_back(value);
					colIdx++;
				}
			}
		}
	}

	file.close();
	return true;
}


bool View::onClick(gui::Button* pBtn) {

	saveAs(currentPathXML);

	if (pBtn == &_generisiTXT) {
		pokreniSolver();
	}


	std::vector<std::string> names;
	std::vector<std::vector<double>> numbers;

	const char* userProfile = getenv("USERPROFILE");
	std::string path = getenv("USERPROFILE");
	path += "\\modelSolver\\rezultat.txt";

	extractNumbers(path, names, numbers);

	plotter.reset();

	for (int i = 1; i < names.size(); ++i){
		plotter.addFunction(numbers[0].data(), numbers[i].data(), numbers[0].size(), 2, td::LinePattern::Solid, names[i]);
		plotter.fitToWindow();
	}

	return false;
}



void View::pokreniSolver(){

	xml::FileParser p;
	p.parseFile(currentPathXML);
	auto root = p.getRootNode();

	td::String s; //treba string builder ali ovo je samo privremeno svakako
	const char* userProfile = getenv("USERPROFILE");
	s = "cd ";
	s += userProfile;
	s += "\\modelSolver & .\\modelSolver.exe ";
	s += root.getAttrib("type")->getValue();
	s += " ";
	s += root.getAttrib("domain")->getValue();
	s += " \"";
	s += currentPathXML;
	s += "\" rezultat.txt ";
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
			pozComment = line.find("<!--", poz);
			if(pozComment != -1)
				break;
			return true;
		}
		
		pozComment = line.find("<!--", poz);
		pozStart = line.find("<", poz);
		if (pozStart == pozComment && pozComment != -1) pozStart = line.find("<", pozComment + 1);
		pozEnd = line.find(">", poz);

		if (pozStart != -1 && line[pozStart + 1] == '/') {//element is closing tag (comment is independent, ignore)

			if (pozComment != -1) {
				poz = pozComment;
				bool getSucces = getComment(comment, alone);
				alone = false;
				return getSucces;
			}

			//komentara nema, samo nadi kraj elementa i zapocni ponovo
			while (poz = line.find(">", poz), (poz == -1 && !file.eof()))
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
				alone = false;
				return true;
			}

		}

		if (pozComment < pozStart || pozStart == -1) { //komenatar prije pocetka elementa
			alone = true;
			break;
		}
		//komentar iza elementa

		alone = false;
		break;

	}


	pozComment += 4;
	int poz2;
	

	while ((poz2 = line.find("-->", poz)), poz2 == -1) {
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

	bool special_case = false;
	bool skip_newline = false, dont_indent = false;;
	enum class closeTags{none, model, group, iff} closeTag = closeTags::none;

	std::vector<td::String> skipAtribs;

	if (node->getName().cCompareNoCase("Model") == 0)
		closeTag = closeTags::model;

	if (node->getName().cCompareNoCase("Group") == 0)
		closeTag = closeTags::group;
	
	if (node->getName().cCompareNoCase("Var") == 0 || node->getName().cCompareNoCase("Param") == 0) {
		result << tab_storage->c_str();

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

		special_case = true;
		skipAtribs.push_back("val");
		skipAtribs.push_back("name");
	}
	
	
	if (node->getName().cCompareNoCase("Then") == 0) {
		dont_indent = true;
		special_case = true;
		skip_newline = true;
	}


	bool ifType = false;

	if (node->getName().cCompareNoCase("Else") == 0) {
		tab_storage->resize(--tabs, '\t');
		result << tab_storage->c_str() << "else";
		special_case = true;
		ifType = true;
	}


	if (node->getName().cCompareNoCase("Eq") == 0) { // ako se radi o uslovnoj jednacini ispisati kao if
		skipAtribs.push_back("cond");
		auto atr = node.getAttrib("cond");
		if(atr != nullptr){
			result << tab_storage->c_str() << "if " << atr->getValue();
			special_case = true;
			closeTag = closeTags::iff;
			ifType = true;
		}
	}
	else if(!special_case) 
		result << tab_storage->c_str() << node->getName() << ':';

		
	//printanje argumenata

	auto atr = node.getAttrib("fx");
	skipAtribs.push_back("fx");
	if (atr != nullptr) {
		if (ifType)
			result << "\n";
		result << tab_storage->c_str() << atr->getValue();
		atr = node.getAttrib("w");
		skipAtribs.push_back("w");
		if (atr != nullptr)
			result << " [" << atr->getValue() << ']';
		skip_newline = false;
	}


	for (auto & var : node->attribs) {

		const char* debug = var.getName().c_str();

		for  (auto &atSkip : skipAtribs)
			if (atSkip.cCompareNoCase(var.getName().c_str()) == 0)
				goto END_LOOP;
		

		if (!skip_newline)
			result << '\n';
		else 
			skip_newline = false;
			
		result << tab_storage->c_str() << '\t' << var.getName() << " = " << var.value;
	END_LOOP:
		;
	}

	
	
PRINT_CHILDREN:
	if (!comment.isNull()) 
		result << "\t//" << comment;
	if (!skip_newline)
		result << '\n';

	auto &it = node.getChildNode();
	while (it.isOk()) {
		if(dont_indent)
			printNode(result, it, tabs);
		else
			printNode(result, it, tabs+1);
		++it;
	}



	static const char* closingTagNames[] = { "" , "endmodel\n", "endgroup\n", "endif\n"};
	
	tab_storage->resize(tabs, '\t');

	if (closeTag != closeTags::none) {
		result << tab_storage->c_str() << closingTagNames[int(closeTag)];
	}



}

bool View::loadFile(const td::String& path) {
	xml::FileParser par;
	StringBuilder s;
	bool x;

	if (path.endsWithCI(".txt", 4)) {
		std::ifstream input(path.c_str());
		if (!input.is_open())
			return false;
		std::string line;
		_jednacineText.clean();
		while (std::getline(input, line)) {
			_jednacineText.appendString(line.c_str());
			_jednacineText.appendString("\n");
		}
		input.close();
		currentPath = path;
		currentPathXML = path.subStr(0, currentPath.length() - 5);
		currentPathXML += ".xml";
		return true;
	}


	if (path.endsWithCI(".xml", 4)) {

		if (!par.parseFile(path) || !getComment(td::String(), x, path.c_str()))
			return false;


		auto& model = par.getRootNode();

		if (model->getName().cCompare("Model") != 0)
			return false;

		printNode(s, model);
		td::String comment;
		while (getComment(comment, x))
			if (!comment.isNull())
				s << "//" << comment << '\n';

		_jednacineText.clean();
		td::String temp;
		s.getString(temp);//moglo bi bez temp da getString vraca string pa da se direktno std::move
		_jednacineText.setText(std::move(temp));


		currentPathXML = path;
		currentPath = path.subStr(0, currentPath.length() - 5);
		currentPath += ".txt";

		return true;

	}

	return false;
}

bool View::save(){
	if (currentPath.isNull()) {
		showAlert("Error", "No file is opened, use 'open' or 'save as'");
		return true;
	}
	saveAs(currentPath);
	return true;
}

bool View::saveAs(td::String path){


	if (path.endsWith(".txt", 4)) {
		std::ofstream output(path.c_str());
		if (!output.is_open())
			return false;
		td::Variant v;
		_jednacineText.getValue(v);
		output << v.strVal().c_str();
		output.close();
		currentPath = path;
		return true;
	}

	if (path.endsWith(".xml", 4)) {
		
		try{
			generateXML(_jednacineText.getText(), path);
		}
		catch (const std::exception& e){
			showAlert("Could not save file", e.what());
			return false;
		}

		currentPathXML = path;
		
		return true;

	}

	return false;

}

