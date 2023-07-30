#include "main.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"
#include "xml/Writer.h"
#include <stdexcept>
#include <array>

/*
class model {
public:
	static const td::String  oznakeJednacina[];
	static const td::String  oznakeModel[];
	static const td::String  oznakeDomene[];
private:

	struct vrijednost {
		td::String ime, komentar;
		double pocetna_vrijednost;
	};

	enum class tipoviModel {
		ODE, DAE, NR, WLS, DIFF
	};

	enum class tipoviJednacina {
		ODE, DAE, NLE, Meas, DIF
	};

	enum class domene {
		real, complex
	};

	td::String ime;
	domene domena = domene::real;

	std::vector<vrijednost> parametri;
	std::vector<vrijednost> varijable;

	model* parentModel;
	bool init_block = false;
	bool post_block = false;


	friend void processLine(td::String equation, const td::String& comment);

public:

	std::vector<model> init_children;
	std::vector<model> post_children;

	model(model* parent) {
		parentModel = parent;
		if (parent == nullptr) return;
		domena = parent->domena;
	}

};

const td::String model::oznakeJednacina[] = { "ODEqs", "DAEqs" , "NLEqs", "MeasEqs", "DIFqs" };
const td::String model::oznakeModel[] = { "ODE", "DAE", "NR", "WLS", "DIFF" };
const td::String model::oznakeDomene[] = { "real", "cmplx" };



model* rootModel = nullptr;

void processLine(td::String equation, const td::String &comment) {
	cnt::PushBackVector<td::String> tokeni;
	equation.split("-/+*", tokeni, true, true);

	static model* currentModel = nullptr;

	if (tokeni.size() > 0){
		if (tokeni[0] == "Model:") {
			if (rootModel == nullptr) {
				rootModel = new model(nullptr);
				currentModel = rootModel;
				return;
			}
			else if (currentModel->init_block || currentModel->post_block) {
				std::vector<model>& kontener(currentModel->init_block ? currentModel->init_children : currentModel->post_children);
				kontener.emplace_back(currentModel);
				currentModel = &(kontener.back());
				return;
			}
			else
				throw std::logic_error("Submodels can only be created inside Init and PostProc blocks");
		}
	}



	for (int i = 0; i < tokeni.size(); ++i){

	}

}

td::UTF8 toUpper(td::UTF8 c) {
	
}

*/
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
	bacaj += "2.xml";
	xml::Writer w(bacaj);
	w.startDocument();

	static const td::String keywords[] = {"IF", "ELSE", "ENDIF", "SIGNAL", "THEN:", "MODEL:", "ENDMODEL", "TYPE=", "DOMAIN=", "NAME=", "EPS=", "METHOD=", "VARS:", "VARIABLES:", "PARS:", "PARAMATERS:", "PARAMS:", "INIT:", "ENDINIT", "ODE:", "NL:", "POSTPROC:", "MEAS:"};
	enum{iff, elsee, endiff, signal, then, model, endmodel, type, domain, name, eps, method, vars, variables, pars, paramaters, params, init, endinit, ode, nl, postproc, meas};

	enum class inputModes{none, variables, paramaters, equations};
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
			openNode("PostProc", inputModes::none);
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
			openNode("MEASEqs", inputModes::equations);
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


		//atribute keywords
		


		/*
		moze ovo umjesto puno ifova ali sporije je

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
		bool isAtribute = false;
		bool commentIsNull = comment.isNull() ? true : false;

		if (startsWith(linije[i], keywords[type])) {
			w.attribute("type", linije[i].trimLeft());
			isAtribute = true;
		}

		if (startsWith(linije[i], keywords[domain])) {
			w.attribute("domain", linije[i].trimLeft());
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
			if (!commentIsNull && !comment.isNull())
			continue;
		}


		//other



		if (startsWith(linije[i], keywords[iff])) {
			w.startElement("Eq");
			w.attribute("cond", linije[i].trimLeft());
			w.startElement("Then");
		}

		if (startsWith(linije[i], keywords[signal])) {
			if (linije[i].trimLeft().getAt(0) == '=')
				w.attribute("signal", linije[i].trimLeft().subStr(1, linije[i].length() - 1).trimLeft());
			else
				w.attribute("signal", "LowLimit");
			
			
		}

		if (startsWith(linije[i], keywords[elsee])) {
			w.endElement();
			w.startElement("Else");
		}

		if (startsWith(linije[i], keywords[endiff])) {
			w.endElement();
		}

		if (startsWith(linije[i], keywords[then])) {
			
		}

		//user didnt enter a keyword

//		if (inputMode == inputModes::none)
//				throw std::logic_error(td::String("unknown keyword \"").c_str());

		attributeInput = false;

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
			w.startElement("Eq");
			w.attribute("fx", linije[i]);
			w.endElement();
		}

		if (!comment.isNull()) {
			w.comment(comment.c_str());
			comment.clean();
		}

	}
	
	
	w.endDocument();
	w.close();



}
