#include "View.h"
#include "td/String.h"
#include "td/Variant.h"
#include "td/StringConverter.h"
#include "gui/NumericEdit.h"

#include "xml/Writer.h"

#include <cstdlib>
#include <Windows.h>

td::Variant& View::ukiniKomentare(td::Variant& var)
{
	td::String s = var.toString();

	td::UINT4 poz = 0;

	td::String pomocna;
	int noviRed;

	while (poz = s.findCI("//", poz), poz!=-1) {
		//s = s.subStr(0, poz) + s.subStr(s.findCI("\n", poz), s.length()); ne mogu pronaci header u kojem se nalazi ovaj operator + linker error svaki put izbaci
		noviRed = s.findCI("\n", poz);
		pomocna = "";
		if(noviRed != -1)
			pomocna = s.subStr(noviRed, s.length());
		s = s.subStr(0, poz - 1);
		s += pomocna;
	}


	var = s;
	return var;
}


int View::uzmiKomentar(int pocetak, td::String& str,td::String& komentar)
{
	int komentarPoz = str.findCI("//", pocetak);
	int kraj = str.findCI("\n", pocetak);
	if (kraj == -1)
		kraj = str.length() + 1;
	if (komentarPoz < kraj && komentarPoz != -1) 
		komentar = str.subStr(komentarPoz, kraj);
	

	return kraj;
}

View::View() : hLayout(1), vLayout(13), jednacinaLabel("Jednacina:"), konstLabel("Konstatne velicine: "), varLabel("Varijable: "), _generisi("Generisi XML i rezultat.txt"), dugmadi(1), opcije(5), tipLabel("Tip: "), domenaLabel("Domena: "), timeLayout(7), 
starLabel("pocetno vrijeme simulacije"), endLabel("krajnje vrijeme simulacije"), korakLabel("korak: "), _start(td::DataType::decimal2), _end(td::DataType::decimal2), _step(td::DataType::decimal2)
{


	//hLayout.appendLayout(vLayout);
	
	td::String tipovi[] = { "ODE", "DAE", "NR", "WLS", "DIFF" };
	td::String domene[] = { "real", "complex" };

	domena.addItems(domene, 2);
	tip.addItems(tipovi, 5);

	opcije.append(tipLabel);
	opcije.append(tip);

	tip.setValue(0);
	domena.setValue(0);

	opcije.append(domenaLabel);
	opcije.append(domena);
	opcije.appendSpace(22);

	_end.setText("10");
	_step.setText("0.01");

	vLayout.appendLayout(opcije);
	timeLayout << starLabel << _start << endLabel << _end << korakLabel << _step;
	timeLayout.appendSpace(20);
	vLayout.appendSpace(20);
	vLayout.appendLayout(timeLayout);
	vLayout.appendSpace(30);

	vLayout.append(varLabel, td::HAlignment::Left);
	vLayout.append(_variablesText);

	_variablesText.setText("x=0");

	vLayout.append(konstLabel, td::HAlignment::Left);
	vLayout.append(_konstText);

	_konstText.setText("c=5");

	vLayout.append(jednacinaLabel, td::HAlignment::Left);
	vLayout.append(_jednacineText);

	_jednacineText.setText("x'=c");

	dugmadi.append(_generisi);

	vLayout.appendSpace(50);
	vLayout.appendLayout(dugmadi);
	vLayout.appendSpace(10);

	
	
	vLayout.setMargins(20, 20);
	setLayout(&vLayout);

	
}


bool View::onClick(gui::Button* pBtn)
{

	if (pBtn == &_generisi) {
		td::Variant v;
		_jednacineText.getValue(v);


		td::String greska;
		int err;

		err = dodajParametre(_konstText, _konst, true);
		err = dodajParametre(_variablesText, _vars, false);


		generisiXml();
	}
	return false;
}


int View::dodajParametre(gui::TextEdit& t, std::vector<parametar>& kontener, bool requireValue){
	td::Variant v;
	t.getValue(v);
	td::String s = v.toString();
	kontener.clear();

	td::String ime, komentar;
	double val;

	int kraj = 0, jednakoPoz, komentarPoz, pocetak = 0;

	while (kraj < s.length()) {
		komentar = "";
		kraj = s.findCI("\n", pocetak);
		if (kraj == -1) kraj = s.length() + 1;
		komentarPoz = uzmiKomentar(pocetak, s, komentar);


		jednakoPoz = s.findCI("=", pocetak);
		if (jednakoPoz == -1 || jednakoPoz > komentarPoz) {
			if (requireValue)
				return pocetak;

			ime = s.subStr(pocetak, komentarPoz -1);
			val = -1;
		}
		else {
			ime = s.subStr(pocetak, jednakoPoz-1);
			val = s.subStr(jednakoPoz + 1, komentarPoz - 1).toDouble();
		}

		kontener.emplace_back(std::move(ime),val, std::move(komentar));
		//provjeriti da li je ime prazno i da li se podudara sa prethodnim imenima, ako da return pocetak
		pocetak = kraj;
	}

	return 0;
}


void View::generisiXml(){
	const char* oznake[] = { "ODEqs", "DAEqs" , "NLEqs", "MeasEqs", "DIFqs"};
	const char* userProfile = getenv("USERPROFILE");
	td::String path(userProfile);
	path += "\\modelSolver\\test.xml";
	xml::Writer w(path);
	w.startDocument();

	w.startNode("Model");

	
	w.attribute("type", tip.getSelectedText());
	w.attribute("domain", domena.getSelectedText());
	w.attribute("name", "test");

	char* parTip = "Var";

	auto fun = [&w, &parTip](parametar& var) {
		w.startNode(parTip);
		w.attribute("name", var.ime);
		w.attribute("val", var.vrijednost);
		w.endNode();
		if(!var.komentar.isNull())
			w.comment(var.komentar.c_str());
	};

	
	w.startNode("Vars");
	std::for_each(_vars.begin(), _vars.end(), fun);
	w.endNode();

	parTip = "Param";
	w.startNode("Params");
	std::for_each(_konst.begin(), _konst.end(), fun);
	w.endNode();

	w.startNode(oznake[tip.getSelectedIndex()]);

	td::String komentar("");
	td::Variant v;
	_jednacineText.getValue(v);
	td::String jednacine = v.toString();

	int pocetak = 0, kraj = 0, komentarPoz;
	int nest = 0;

	//jednacine.removeWhiteSpaces("3", 2); ne razumijem na koji nacin radi funckija ali treba ukinit white spaces

	while (pocetak < jednacine.length()) {
		komentar = "";
		kraj = jednacine.findCI("\n", pocetak);
		if (kraj == -1) kraj = jednacine.length() + 1;
		komentarPoz = uzmiKomentar(pocetak, jednacine, komentar);

		if ((kraj - pocetak) >= 3 && cCompareLimit(jednacine.c_str() + pocetak, "if(", 3) == 0) {
			++nest;
			w.startNode("Eq");
			int condEnd = jednacine.findCI(")", pocetak);
			if (condEnd == -1)
				condEnd = komentarPoz;
			w.attribute("cond", jednacine.subStr(pocetak + 3, condEnd-1));
			w.startNode("Then");
			goto continue_loop;

		}
		if ((kraj-pocetak) >= 5 && cCompareLimit(jednacine.c_str() + pocetak, "endif", 5) == 0) {
			--nest;
			w.endNode();
			w.endNode();
			goto continue_loop;
		}
		if ((kraj - pocetak) >= 4 && cCompareLimit(jednacine.c_str() + pocetak, "else", 4) == 0) {
			w.endNode();
			w.startNode("Else");
			goto continue_loop;
		}

		w.startNode("Eq");
		w.attribute("fx", jednacine.subStr(pocetak, komentarPoz-1));
		w.endNode();

	continue_loop:
		if(!komentar.isNull())
			w.comment(komentar.c_str());
		pocetak = kraj+1;
	}

	w.endDocument();
	w.close();

	pokreniSolver();

}


void View::pokreniSolver(){

	td::String s;
	const char* userProfile = getenv("USERPROFILE");
	s = "cd ";
	s += userProfile;
	s += "\\modelSolver & .\\modelSolver.exe ";
	s += tip.getSelectedText();
	s += " ";
	s += domena.getSelectedText();
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

int View::cCompareLimit(const char* s1, const char* s2, int len){
	while ((*s1++ - *s2++) == 0 && (len > 0)) --len;
	return len;
}


