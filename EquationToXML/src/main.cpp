#include "main.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"
#include "main.h"

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
	domene domena;

	std::vector<vrijednost> parametri;
	std::vector<vrijednost> varijable;


} mainModel;

const td::String model::oznakeJednacina[] = { "ODEqs", "DAEqs" , "NLEqs", "MeasEqs", "DIFqs" };
const td::String model::oznakeModel[] = { "ODE", "DAE", "NR", "WLS", "DIFF" };
const td::String model::oznakeDomene[] = { "real", "cmplx" };


int uzmiKomentar(int pocetak, td::String& str, td::String& komentar)
{
	int komentarPoz = str.findCI("//", pocetak);
	int kraj = str.findCI("\n", pocetak);
	if (kraj == -1)
		kraj = str.length() + 1;
	if (komentarPoz < kraj && komentarPoz != -1)
		komentar = str.subStr(komentarPoz, kraj);
	else
		komentarPoz = kraj;


	return komentarPoz;
}

void processLine(td::String equation) {
	int jednako = equation.find("=", 0);
	td::String s;
	if (jednako) {
		if (equation.find("=", jednako + 1))
			throw std::exception();
	}

	cnt::PushBackVector<td::String> tokeni;
	equation.split("-/+*", tokeni, true, true);


}

void generateXML(td::String equations, td::String output_path) {

	td::UTF8 jen(' ');
	const td::UTF8* jenn = &jen;
	td::UTF8 dva(' ');


	equations.removeWhiteSpaces(jenn, dva);
	const char* test = equations.c_str();

	int i = 3 + 2;

	return;

	cnt::PushBackVector<td::String> linije;
	equations.split("\n;", linije, true, true);

	for (int i = 0; i < linije.size(); i++){
		processLine(linije[i]);
	}
	

	

	/*
	* 
	* 	xml::Writer w(path);
	w.startDocument();

	w.startNode("Model");

	
	w.attribute("type", tip.getSelectedText());
	w.attribute("domain", domena.getSelectedText());
	w.attribute("name", "test");
	cnt::PushBackVector<td::String> token;
	td::String komentar;

	//jednacine.removeWhiteSpaces("3", 2); ne razumijem na koji nacin radi funckija ali treba ukinit white spaces

	while (pocetak < jednacine.length()) {
		komentar = "";
		//kraj = uzmiKomentar(pocetak, jednacine, komentar);

		jednacine.split("-/*+", token, true, true);
	}
*/
}
