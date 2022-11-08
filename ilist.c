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
 * Soubor ilist.c
 *
 * Soubor s implementaci instrukcniho listu.
 */


#include <stdio.h>
#include <malloc.h>
#include "ilist.h"

void listInit(tIList *L)
// funkce inicializuje seznam instrukci
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}
  
void listFree(tIList *L)
// funkce dealokuje seznam instrukci
{
  tListItem *ptr;
  while (L->first != NULL)
  {
    ptr = L->first;
    L->first = L->first->nextItem;
    // uvolnime celou polozku
    free(ptr);
  }
}

void listInsertLast(tIList *L, int type, void* address1, void* address2, void* address3)
// vlozi novou instruci na konec seznamu
{
  tInstr newInstr;
  newInstr.instType = type;
  newInstr.addr1 = address1;
  newInstr.addr2 = address2;
  newInstr.addr3 = address3;

  tListItem *newItem;
  newItem = malloc(sizeof (tListItem));
  newItem->Instruction = newInstr;
  newItem->nextItem = NULL;
  if (L->first == NULL)
     L->first = newItem;
  else
     L->last->nextItem=newItem;
     L->last=newItem;
}

void listFirst(tIList *L)
// zaktivuje prvni instrukci
{
  L->active = L->first;
}

void listNext(tIList *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  L->active = L->active->nextItem;
}

void listGoto(tIList *L, void *gotoInstr)
// nastavime aktivni instrukci podle zadaneho ukazatele
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  L->active = (tListItem*) gotoInstr;
}

void *listGetPointerLast(tIList *L)
// vrati ukazatel na posledni instrukci
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  return (void*) L->last;
}

tInstr *listGetData(tIList *L)
// vrati aktivni instrukci
{
  if (L->active == NULL)
  {
    printf("Chyba, zadna instrukce neni aktivni");
    return NULL;
  }
  else return &(L->active->Instruction);
}
