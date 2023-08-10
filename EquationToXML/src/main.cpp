#include "../inc/generateXML.h"
#include "../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>


#include "td/String.h"


#include "xml/Writer.h"
#include <stdexcept>
#include <array>

#include<unordered_map>
#include<map>
#include <utility>


void baseNode::printNode(xml::Writer& w) {
	w.startNode(this->getName());
	for each (auto &at in this->attribs)
		w.attribute(at.first.c_str(), at.second);



	if (nodes.size() == 0)
		w.endNode();

	if (!comment.isNull()) {
		cnt::PushBackVector<td::String> coms;
		comment.split("\n", coms, true, true);
		w.comment(coms[0].c_str());
		for (int i = 1; i < coms.size(); ++i)
			w.commentInNewLine(coms[i].c_str());
	}

	if (nodes.size() != 0) {

		for each (baseNode * var in nodes)
			var->printNode(w);

		w.endNode();
	}

	


}

size_t baseNode::addLine(std::vector<std::pair<td::String, td::String>> lines, size_t startLine){
	while (startLine < lines.size()) {
		if (lines[startLine].first.isNull()) {
			addComment(lines[startLine].second);
			++startLine;
			continue;
		}

		int pozEq = lines[startLine].first.find("=");
		
		bool found = false;
		if (pozEq != -1) {
			td::String keyword = lines[startLine].first.subStr(0, pozEq - 1).trimRight();
			for each (auto & var in attributeKeywords) {
				if (var == keyword) {
					setAttrib(lines[startLine].first.subStr(0, pozEq - 1).trimRight(), lines[startLine].first.subStr(pozEq + 1, -1).trimLeft());
					addComment(lines[startLine].second);
					found = true;
					break;
				}
			}
		}
		if (found) {
			++startLine;
			continue;
		}
		baseNode* child = nullptr;
		if (nodeAction(lines[startLine].first, child)) {
			if (child != nullptr) {
				child->addComment(lines[startLine].second);
				startLine = child->addLine(lines, startLine+1);
				continue;
			}
			addComment(lines[startLine].second);
			++startLine;
			continue;
		}

		if (child != nullptr) {
			startLine = child->addLine(lines, startLine);
			continue;
		}

		return startLine;
		
	}
}

void baseNode::addComment(const td::String& comment){
	if (comment.isNull())
		return;
	if (!this->comment.isNull())
			this->comment += "\n";
	this->comment += comment;
}

void baseNode::addComment(td::String&& comment){
	if (comment.isNull())
		return;
	if (this->comment.isNull()) 
		this->comment = comment;
	else {
		this->comment += "\n";
		this->comment += comment;
	}
}

void baseNode::processCommands(const td::String& text) {
	std::vector<std::pair<td::String, td::String>> lines;
	cnt::PushBackVector<td::String> l;
	text.split("\n;", l, true, true);
	int poz;
	for (int i = 0; i < l.size(); ++i){
		poz = l[i].find("//");
		if(poz == -1)
			lines.emplace_back<std::pair<td::String, td::String>>({l[i].trim(), ""});
		else if(poz != 0)
			lines.emplace_back<std::pair<td::String, td::String>>({ l[i].subStr(0,poz-1).trim(), l[i].subStr(poz + 2, -1)});
		else
			lines.emplace_back<std::pair<td::String, td::String>>({ "", l[i].subStr(poz + 2, -1) });
	}
	addLine(lines);
}


class nameNode : public baseNode {
	td::String name;
public:
	nameNode(const td::String &name) : name(name) {}
	nameNode(td::String &&name) : name(name) {}

	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		return false;
	}

	inline const char* getName() override{
		return name.c_str();
	}
};



template<typename containerName, typename nodeType>
class containerNode : public baseNode {
public:
	bool nodeAction(const td::String& command, baseNode*& newChild) override{

			if (command.getLastChar() == ':') return false;
			if (command.beginsWithCI("end")) return false;

			newChild = new nodeType;
			nodes.push_back(newChild);

			return false;
	}
	
	inline const char* getName() override {
		return containerName::val;
	}

};

template<typename variableName>
class variableNode : public baseNode {
	bool added = false;
public:
	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (added) return false;
		added = true;

		const char* smece = command.c_str();

		int pozEq = command.find("=");
		if (pozEq == -1) {
			setAttrib("name", command);
			return true;
		}

		setAttrib("name", command.subStr(0, pozEq - 1).trimRight());
		setAttrib("val", command.subStr(pozEq + 1, -1).trimLeft());
		
		return true;
	}

	inline const char* getName() override {
		return variableName::val;
	}
};

class singleEquation : public baseNode {
	bool added = false;
public:
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (added)
			return false;
		//dodaj provjeru da li se radi o if-u
		setAttrib("fx", command);
		added = true;
		return true;
	}
	inline const char* getName() override {
		return "Eq";
	}
};

struct varsName { static const char* val; };
const char* varsName::val = "Vars";

struct varName { static const char* val; };
const char* varName::val = "Var";

struct paramsName { static const char* val; };
const char* paramsName::val = "Params";

struct paramName { static const char* val; };
const char* paramName::val = "Param";

struct NLEName { static const char* val; };
const char* NLEName::val = "NLEqs";

struct ODEName { static const char* val; };
const char* ODEName::val = "ODEqs";

struct postProcName { static const char* val; };
const char* postProcName::val = "PostProc";

typedef containerNode<varsName, variableNode<varName>> varsNode;
typedef containerNode<paramsName, variableNode<paramName>> paramsNode;
typedef containerNode<NLEName, singleEquation> NLEquationsNode;
typedef containerNode<ODEName, singleEquation> ODEquationsNode;
typedef containerNode<postProcName, singleEquation> postProcNode;


class initNode : public baseNode {
public:
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {

		if (command.cCompareNoCase("model:") == 0) {
			nodes.push_back(new modelNode());
			newChild = nodes.back();
			return true;
		}
		
		return false;
	}
	inline const char* getName() override {
		return "Init";
	}
};


modelNode::modelNode(td::String text){
	processCommands(text);
}

bool modelNode::nodeAction(const td::String& command, baseNode*& newChild){

	if (command.beginsWithCI("end")) return false;


	if (command.getLastChar() == ':') { //radi se o novom elementu
		if (command.cCompareNoCase("Vars:") == 0)
			nodes.push_back(new varsNode());
		else if (command.cCompareNoCase("Params:") == 0)
			nodes.push_back(new paramsNode());
		else if (command.cCompareNoCase("model:") == 0) 
			return true;
		else if (command.cCompareNoCase("NLEqs:") == 0)
			nodes.push_back(new NLEquationsNode());
		else if (command.cCompareNoCase("ODEqs:") == 0)
			nodes.push_back(new ODEquationsNode());
		else
			nodes.push_back(new nameNode(command.subStr(0, command.length() - 2)));

		newChild = nodes.back();
		return true;
	}

	
}



void generateXML(const td::String& equations, const td::String& output_path) {
	modelNode m(equations);

	td::String bacaj(output_path.subStr(0, output_path.length() - 5));
	bacaj += "test.xml";
	xml::Writer w(bacaj);
	w.startDocument();

	m.printNode(w);

	w.endDocument();
	w.close();
}

/*
void splitComment(td::String& equation, td::String& comment)
{
	int komentarPoz = equation.find("//");
	if (komentarPoz == -1)
		return;
	int kraj = equation.find("\n");
	if (kraj == -1)
		kraj = equation.length() - 1;

	if (comment.isNull())
		comment = std::move(equation.subStr(komentarPoz + 2, kraj));
	else {
		comment += ", ";
		comment += equation.subStr(komentarPoz + 2, kraj);
	}
	if(komentarPoz != 0)
		equation = std::move(equation.subStr(0, komentarPoz - 1));
	else
		equation.clean();
}

bool startsWith(td::String& string, const td::String& word) {
	if (string.length() < word.length())
		return false;

	int kraj = string.findCI(word.end()-1);
	if (kraj == -1)
		return false;
	td::String copy = string.subStr(0, word.length()-1);
	copy.setAt(word.length()-1, word.getLastChar());
	copy.toUpper();
	const char* kopp = copy.c_str();
	const char* workk = word.c_str();
	if(copy.compare(word) != 0)
		return false;
	string = std::move(string.subStr(kraj+1, string.length()-1));
	return true;

}





void generateXML(const td::String &equations, const td::String &output_path) {

	cnt::PushBackVector<td::String> linije;
	equations.split("\n;", linije, true, true);
	td::String comment;
	bool rootModel = false;
	bool closeNode = false;
	bool attributeInput = false;
	td::String bacaj(output_path.subStr(0,output_path.length()-5));
	bacaj += "test.xml";
	xml::Writer w(bacaj);
	w.startDocument();

	static const td::String keywords[] = {"IF", "ELSE", "ENDIF", "SIGNAL", "THEN:", "MODEL:", "ENDMODEL", "TYPE=", "DOMAIN=", "NAME=", "EPS=", "DT=", "METHOD=", "VARS:", "VARIABLES:", "PARS:", "PARAMATERS:", "PARAMS:", "INIT:", "ENDINIT", "ODE:", "NL:", "POSTPROC:", "MEAS:", "GROUP:", "ENDGROUP"};
	enum{iff, elsee, endiff, signal, then, model, endmodel, type, domain, name, eps, dt, method, vars, variables, pars, paramaters, params, init, endinit, ode, nl, postproc, meas, group, endgroup};

	enum class inputModes{none, variables, paramaters, equations, equationsMeas};
	inputModes inputMode = inputModes::none;

	auto openNode = [&w, &closeNode, &inputMode, &attributeInput](const char* name, inputModes mod) {
		if (closeNode)
			w.endElement();
		else
			closeNode = true;
		w.startElement(name);
		attributeInput = true;
		inputMode = mod;
	};

	for (int i = 0; i < linije.size(); i++){
		splitComment(linije[i], comment);


		const char* smece = linije[i].c_str();
		const char* smece2 = comment.c_str();

		linije[i] = std::move(linije[i].trim());

		if (linije[i].isNull())
			if(comment.isNull())
				continue;
			else {
				if (!attributeInput){
					w.commentInNewLine(comment.c_str());
					comment.clean();
				}
				continue;
			}


// keywords that change input mode

		if (startsWith(linije[i], keywords[vars]) || startsWith(linije[i], keywords[variables])) {
			openNode("Vars", inputModes::variables);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[init])) {
			openNode("Init", inputModes::none);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[postproc])) {
			openNode("PostProc", inputModes::equations);
			closeNode = false;
			--i; continue;
		}

		if (startsWith(linije[i], keywords[pars]) || startsWith(linije[i], keywords[params]) || startsWith(linije[i], keywords[paramaters])) {
			openNode("Params", inputModes::paramaters);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[ode])) {
			openNode("ODEqs", inputModes::equations);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[nl])) {
			openNode("NLEqs", inputModes::equations);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[meas])) {
			openNode("MEASEqs", inputModes::equationsMeas);
			--i; continue;
		}

		if (startsWith(linije[i], keywords[model])) {
			if (!rootModel){
				rootModel = true;
				//pronadi koristi li se conj
			}
			closeNode = false;
			w.startElement("Model");
			inputMode = inputModes::none;
			attributeInput = true;

			--i; continue;
		}

		if (startsWith(linije[i], keywords[endmodel])) {
			if (closeNode) {
				w.endElement();
			}
			w.endElement();
			closeNode = true;
			inputMode = inputModes::none;
			continue;
		}

		if (startsWith(linije[i], keywords[endinit])) {
			closeNode = false;
			w.endElement();
			inputMode = inputModes::none;
			continue;
		}

		if (startsWith(linije[i], keywords[group])) {
			openNode("Group", inputMode);
			continue;
		}

		if (startsWith(linije[i], keywords[endgroup])) {
			w.endElement();
			continue;
		}

		//atribute keywords
		


		/*
		moze ovo umjesto puno ifova jer ime atributa se uvijek poklapa sa komandom ali sporije je

		static const std::array<int, 5> atributi = {type, domain, name, eps, method};
		for (int j = 0; j <atributi.size() ; ++j)
		{
			if (startsWith(linije[i], keywords[atributi[j]])) {
				td::String s(keywords[atributi[j]]);
				s.setAt(s.length() - 2, '\0');
				s.toLower();
				w.attribute(s.c_str(), linije[i].trimLeft());
				j = atributi.size()+1
			}
			if(j == (atributi.size()+1))
				continue;
		}
		*/
		/*
		bool isAtribute = false;

		if (startsWith(linije[i], keywords[type])) {
			w.attribute("type", linije[i].trimLeft());
			isAtribute = true;
		}

		if (startsWith(linije[i], keywords[domain])) {
			w.attribute("domain", linije[i].trimLeft());
			isAtribute = true;
		}

		if (startsWith(linije[i], keywords[dt])) {
			w.attribute("dT", linije[i].trimLeft());
			isAtribute = true;
		}

		if (startsWith(linije[i], keywords[name])) {
			w.attribute("name", linije[i].trimLeft());
			isAtribute = true;
		}

		if (startsWith(linije[i], keywords[eps])) {
			w.attribute("eps", linije[i].trimLeft());
			isAtribute = true;;
		}

		if (startsWith(linije[i], keywords[method])) {
			w.attribute("method", linije[i].trimLeft());
			isAtribute = true;;
		}

		if (isAtribute) {
			goto ADD_COMMENT;
		}



		attributeInput = false;
			



		//other



		if (startsWith(linije[i], keywords[iff])) {
			w.startElement("Eq");
			w.attribute("cond", linije[i].trimLeft());
			w.startElement("Then");
			goto ADD_COMMENT;
		}

		if (startsWith(linije[i], keywords[signal])) {
			if (linije[i].trimLeft().getAt(0) == '=')
				w.attribute("signal", linije[i].trimLeft().subStr(1, linije[i].length() - 1).trimLeft());
			else
				w.attribute("signal", "LowLimit");
			goto ADD_COMMENT;
			
		}

		if (startsWith(linije[i], keywords[elsee])) {
			w.endElement();
			w.startElement("Else");
			goto ADD_COMMENT;
		}

		if (startsWith(linije[i], keywords[endiff])) {
			w.endElement();
			w.endElement();
			goto ADD_COMMENT;
		}

		if (startsWith(linije[i], keywords[then])) {
			goto ADD_COMMENT;
		}

		//user didnt enter a keyword

//		if (inputMode == inputModes::none)
//				throw std::logic_error(td::String("unknown keyword \"").c_str());

		

		if (inputMode == inputModes::variables || inputMode == inputModes::paramaters) {
			int poz = linije[i].find("=", 0);
			w.startElement((inputMode == inputModes::variables) ? "Var" : "Param");
			if (poz == -1) {
				w.attribute("name", linije[i].trim());
			}
			else {
				w.attribute("name", linije[i].subStr(0, poz-1).trim());
				w.attribute("val", linije[i].subStr(poz+1, linije[i].length()-1).trim());
			}
			w.endElement();
		}
		
		if (inputMode == inputModes::equations) {
			if (closeNode)
				w.endElement();
			closeNode = true;
			w.startElement("Eq");
			w.attribute("fx", linije[i]);
			attributeInput = true;
		}
		if (inputMode == inputModes::equationsMeas) {
			w.startElement("Eq");
			int poz1 = linije[i].find("[");
			int poz2 = linije[i].find("]");
			if (poz1 == -1 || poz2 == -1)
				throw("Meas equations must contain [weight] in equation line");
			td::String rest(linije[i].subStr(poz2, linije[i].length()-1));
			w.attribute("w", linije[i].subStr(poz1,poz2));
			linije[i] = linije[i].subStr(0, poz1 - 1);
			linije[i] += rest;
			w.attribute("fx", linije[i]);
			w.endElement();
		}

		ADD_COMMENT:

		if (!comment.isNull()) {
			w.comment(comment.c_str());
			comment.clean();
		}

	}
	
	
	w.endDocument();
	w.close();



}
*/