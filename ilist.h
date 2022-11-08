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
 * Soubor ilist.h
 *
 * Soubor s implementaci instrukcniho listu.
 */


#ifndef _ILIST_H
#define _ILIST_H

#include "ial.h"


//typy instrukci

#define I_STOP     0    //posledni instrukce
#define I_START    1    //prvni instrukce
#define I_PAR      2    //parametry interpretu

//mat operace
#define I_ADD_I   10    //scitani integeru
#define I_ADD_D   11    //scitani doublu
#define I_SUB_I   12    //odcitani integeru
#define I_SUB_D   13    //odcitani doublu
#define I_MUL_I   14    //nasobeni intigru
#define I_MUL_D   15    //nasobeni doublu
#define I_IDIV    16    //celociselne deleni
#define I_DIV     17    //deleni

//porovnavani
#define I_G_I     19    //"vetsi nez" pro integer
#define I_G_D     20    //"vetsi nez" pro double
#define I_G_S     21    //"vetsi nez" pro string
#define I_GE_I    22    //"vetsi nebo rovno" pro integer
#define I_GE_D    23    //"vetsi nebo rovno" pro double
#define I_GE_S    24    //"vetsi nebo rovno" pro string
#define I_L_I     25    //"mensi nez" pro integer
#define I_L_D     26    //"mensi nez" pro double 
#define I_L_S     27    //"mensi nez" pro string
#define I_LE_I    28    //"mensi nebo rovno" pro integer 
#define I_LE_D    29    //"mensi nebo rovno" pro double 
#define I_LE_S    30    //"mensi nebo rovno" pro string 
#define I_NE_I    31    //nerovnost integeru
#define I_NE_D    32    //nerovnost doublu
#define I_NE_S    33    //nerovnost stringu
#define I_E_I     34    //rovnost integeru
#define I_E_D     35    //rovnost doublu
#define I_E_S     36    //rovnost stringu
//obecne instrukce
#define I_INT2DBL 37    //int to double
#define I_KONK    38    //konkatenace
#define I_MOV     39    //prirazeni
#define I_FIND    40    //hledani
#define I_SORT    41    //serazeni
#define I_READ_I  42    //input intigeru
#define I_READ_D  43    //input doublu
#define I_READ_S  44    //input stringu
#define I_WRITE_I 45    //vypis intigeru
#define I_WRITE_D 46    //vypis doublu
#define I_WRITE_S 47    //vypis stringu
#define I_INC     48    //inkrementace
#define I_DEC     49    //dekrementace
#define I_NOT     50    //negace
#define I_INPUT   51    //vypis pro input
//skoky
#define I_FCALL   60    //skok na funkci
#define I_JMP     61    //podmineny skok
#define I_UNJMP   62    //nepodmineny skok	
//navesti                
#define NAV_WHILE 70    //navesti pro while
#define NAV_IF    71    //navesti pro if


typedef struct
{
  int instType;  // typ instrukce
  void *addr1; // adresa 1
  void *addr2; // adresa 2
  void *addr3; // adresa 3
} tInstr;

typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;

typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
} tIList;

void listInit(tIList *L);
void listFree(tIList *L);
void listInsertLast(tIList *L, int, void*, void*, void*);
void listFirst(tIList *L);
void listNext(tIList *L);
void listGoto(tIList *L, void *gotoInstr);
void *listGetPointerLast(tIList *L);
tInstr *listGetData(tIList *L);

#endif
