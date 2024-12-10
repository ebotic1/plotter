ModelMaker - program koji sadrži grafički i tekstualni editor modela jednačina.

Plot (folder) - DLL koji se može uključiti u program. Potrebno je ukljuciti i resurs fajlove ovog foldera u projekat u koji se dodaje.
Sadrži i standalone plotter sa sidebar-om koji prikazuje i mijenja nacrtane linije. Dodan i interface za ucitavanje fajlova

EquationToXML - sadrži klasu koja vrši pretvaranje teksta ili XML fajla u klasu koja opisuje model jednačina. Iz klase je moguče dobiti XML ili tekstualni format.

ToDo:
Greške:
1. Ne učitava model PF_PV_My3.xml
2. Problem sa Limits

Poboljsanja:
1. Kada se javi greška i ispiše se linija u logu, nemoguće naći gdje se ta linija nalazi.
	- Dodati status bar koji će pokazivati trenutnu liniju i kolonu
	- Dodati opciju GoTo Line
	- Dodati mogućnost da se klikom na log sa linikom skoči na liniju u editoru
