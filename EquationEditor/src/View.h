#include "gui/View.h"
#include "gui/TextEdit.h"
#include "gui/VerticalLayout.h"
#include "gui/HorizontalLayout.h"
#include "gui/Label.h"
#include "gui/Button.h"
#include "td/Variant.h"
#include <vector>
#include "gui/ComboBox.h"
#include "gui/NumericEdit.h"

class View : public gui::View {
	gui::HorizontalLayout hLayout, opcije;
	gui::ComboBox tip, domena;
	gui::Label tipLabel, domenaLabel;
	gui::HorizontalLayout timeLayout;
	gui::NumericEdit _start, _end, _step;
	gui::Label starLabel, endLabel, korakLabel;

	gui::VerticalLayout vLayout;
	gui::TextEdit _jednacineText;
	gui::Label jednacinaLabel;
	gui::TextEdit _konstText;
	gui::Label konstLabel;
	gui::TextEdit _variablesText;
	gui::Label varLabel;
	

	gui::HorizontalLayout dugmadi;
	gui::Button _generisiXML, _generisiTXT;

	struct parametar{
		td::String ime;
		double vrijednost;
		td::String komentar;
		parametar(td::String ime, double v, td::String komentar): ime(ime), vrijednost(v), komentar(komentar){}
	};

	std::vector<parametar> _konst, _vars;



	td::Variant& ukiniKomentare(td::Variant &var);
	int uzmiKomentar(int pocetak, td::String& str, td::String& komentar);
	int dodajParametre(gui::TextEdit& t, std::vector<parametar>& kontener, bool requireValue);
	void generisiXml();
	
	
	int cCompareLimit(const char* s1, const char* s2, int len);

	void pokreniSolver();


public:

	View();
	virtual bool onClick(gui::Button* pBtn);

	
};