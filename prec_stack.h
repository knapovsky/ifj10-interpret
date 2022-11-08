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
 * Soubor prec_stack.h
 *
 * Soubor se zasobnikem pro precedencni analyzu.
 */

#ifndef _PPASTACK_H
#define _PPASTACK_H
#include "str.h"

typedef struct tElem {
    struct tElem *ptr;
    struct tElem *ptl;
    int data;
    string attr;
} tElem;	               
                                                           
                                                        
typedef struct { 
   struct tElem *Act;
   struct tElem *First;
} tStack;

void Error(void); // chyba
void InitList (tStack *); // inicializace
void DisposeList (tStack *); //vyèištìní seznamu
void InsertFirst (tStack *, int, string); //vložit na první místo
void First (tStack *); // nastavit prvni jako aktivni
int CopyFirst (tStack *); // vypsat prvni
void DeleteFirst (tStack *); // smazat prvni
void PostInsert (tStack  *, int,string); //vlozit za aktivni
void PreInsert (tStack  *, int,string); //vlozit pred aktivni
int Copy (tStack *)	; //vypsat aktivni
void Actualize (tStack *, int); // zmenit akrivni
void Succ (tStack *); //posunotu aktivitu
int Active (tStack *); //rozeznani aktivity
void FirstTerminal(tStack*); // vraci prvni terminal
int IsTerminal(int); //vrací true pokud je aktivni prvek terminal
int FirstLower(tStack *); //nastavi aktivitu na prvni <
void DelettoLower (tStack *); // smaze ce po i s <
string *CopyPtrAttr (tStack *); //vypise atribut aktivniho prvku
string CopyAttr (tStack *);

#endif
