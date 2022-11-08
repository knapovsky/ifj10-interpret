/**
  * Prekladac imperativniho jazyka IFJ10
  * Projekt do predmetu IFJ a IAL
  * VUT FIT Brno, 2010
  *
  * Tym 5, varianta a/4/I
  * Resitele:
  *   Martin Knapovsky
  *   Pavel Antolik
  *   Ondrej Kratochvil
  *   Jan Myler
  *   Jiri Navratil
  *
  * Soubor const.h
  *
  * konstanty
  */


#define SUCCESS	0	//vse v poradku
#define LEX_ERR	1	//chybna lexikalni struktura programu
#define SYN_ERR	2	//chybna syntakticka struktura programu
#define SEM_ERR	3	//chyba v programu v ramci semantickych kontrol
#define RUN_ERR	4	//chyba za behu programu v ramci interpretace konkretnich dat
									//deleni nulou, na vstupu chybny format ciselne hodnoty atd.
#define INT_ERR	5	//interni chyba interpretu neovlivnena vstupnim programem
	      						//chyba alokace pameti, chyba pri otvirani vstupniho souboru atd.
