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
  * Soubor main.c
  *
  */
 
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "const.h"
#include "ial.h"
#include "ilist.h"
#include "interpret.h"

extern int token;

int main(int argc, char** argv) {
   FILE *in;
   
   if(argc != 2) {
      fprintf(stderr, "Chybne parametry programu!\nZadej jmeno souboru.\n");
      return EXIT_FAILURE;
   }
   else if((in = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "Soubor nelze otevrit!\n");
      return EXIT_FAILURE;
   }
   
   set_source_file(in);    // nastaveni vstupniho souboru v modulu LA
   tGTS* gTS;              // tabulka symbolu
   gts_init(&gTS);         // inicializace tabulky
   tIList list;            // seznam instrukci
   listInit(&list);        // inicializace seznamu instrukci

   int result;             
   result = parse(&gTS, &list);   // provedeni SA
   if(result != SUCCESS)
      return result;

   // volani interpretu
   result = interpret(gTS, &list);

   listFree(&list);
   gts_free(&gTS);         // uvoleni tabulky
   fclose(in);             // zavreni vstupu
   
   return result;  
}
